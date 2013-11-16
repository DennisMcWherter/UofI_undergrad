#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netdb.h>
#include <semaphore.h>

#include "aux_http.h"
#include "aux_socket.h"
#include "priqueue.h"

#define MAX_CACHE 128 /* 128 max items in cache cache */
#define MAX_THREADS 32 /* 32 threads max */
#define MAX_PENDING 500 /* Max pending jobs */

static short done = 0;
pthread_mutex_t mutex;
pthread_mutex_t job_mutex;
pthread_mutex_t hostcheck;

typedef struct _tdata_t_
{
  priqueue_t* jobs;
  priqueue_t* queue;
  sem_t* sem;
} tdata_t;

typedef struct _queue_item_t
{
  char* req;
  void* resp;
  int size;
  unsigned used;
} queue_item_t;

int compare_jobs(const void* elem1, const void* elem2)
{
  if(elem1 == elem2) /* Stop gcc from whining */
    return 0;
  return -1;
}

int match_jobs(const void* elem1, const void* elem2)
{
  if(*((int*)elem1) == *((int*)elem2))
    return 0;
  return -1;
}

int comparer(const void* elem1, const void* elem2){
  queue_item_t* item1 = (queue_item_t*)elem1, * item2 = (queue_item_t*)elem2;
  if(elem1 == NULL || elem2 == NULL)
    return 0;
  return item1->used - item2->used;
}

int match(const void* elem1, const void* key){
  queue_item_t* item1 = (queue_item_t*)elem1;
  const char* x = (const char*)key;
  if(elem1 == NULL || key == NULL)
    return -1;
  return strncmp(item1->req, x, strlen(item1->req)) == 0 ? 0 : -1;
}

void sigINT_handler(int s){
  if(s == SIGINT) {
    done = 1; /* Global variable to shut things down */
  }
}

int checkRead(const char* buffer)
{
  const char end[5] = { '\r', '\n', '\r', '\n', '\0' };
  if(buffer == NULL)
    return 0;
  return strstr(buffer, end) != NULL;
}

void resizeBuffer(char** buffer, int* bufsize, int numtoread, int bytesread)
{
  if(*bufsize - bytesread < numtoread) {
    *bufsize *= 2;
    *buffer = (char*)realloc(*buffer, (*bufsize)*sizeof(char));
  }
}

char* readFromClient(int sockfd)
{
  const int toRead = 256;
  int bytesRead = 0;
  int bufSize = 256;
  int read = 0;
  char* retval = NULL;
  struct timeval timeo;
  fd_set readset;

  timeo.tv_sec  = 5; /* Wait 5s */
  timeo.tv_usec = 0; 
  
  retval = (char*)malloc(bufSize*sizeof(char));
  while(1) {
    FD_ZERO(&readset);
    FD_SET(sockfd, &readset);
    select(sockfd+1, &readset, NULL, NULL, &timeo);
    if(FD_ISSET(sockfd, &readset)) {
      read = recv(sockfd, retval+bytesRead, toRead, 0);
      if(read < 1)
        break;
      bytesRead += read;
      resizeBuffer(&retval, &bufSize, toRead, bytesRead);
    } else if(bytesRead == 0) {
      free(retval);
      return NULL;
    } else {
      retval[bytesRead] = '\0';
      if(checkRead(retval))
        break; /* Nothing to be read & valid ending - assume we go everything? */
    }
  }
  
  return retval;
}

int initHostRequest(const char* buffer)
{
  int retval = -1;
  struct hostent* test = NULL;
  hostAddress* host = NULL;
  
  if(buffer == NULL)
    return -1;

  host = getHostFromHTTPRequest(buffer);

  if(host == NULL)
    return -1;

#if 1
  pthread_mutex_lock(&hostcheck);
  test = gethostbyname(host->hostname);
  if(test == NULL) {
    free(host->hostname);
    free(host->port);
    free(host);
    pthread_mutex_unlock(&hostcheck);
    return -1;
  }
  pthread_mutex_unlock(&hostcheck);
#endif

  retval = newClientSocket(host->hostname, host->port);

  free(host->hostname);
  free(host->port);
  free(host);
  return retval;
}

int chunkSend(int sockfd, char* buffer, int bytes)
{
  const int toSend = 256;
  struct timeval timeo;
  int bytesRemain = bytes;
  fd_set write;

  if(buffer == NULL)
    return 0;

  timeo.tv_sec  = 5; /* Wait 5 seconds - generous */
  timeo.tv_usec = 0;

  while(bytesRemain > 0) {
    FD_ZERO(&write);
    FD_SET(sockfd, &write);
    select(sockfd+1, NULL, &write, NULL, &timeo);
    if(!FD_ISSET(sockfd, &write))
      return 0;
    send(sockfd, buffer+(bytes-bytesRemain), (bytesRemain > toSend) ? toSend : bytesRemain, 0);
    bytesRemain -= toSend;
  }

  return 1;
}

void destroyQueueItem(queue_item_t** item)
{
  free((*item)->req);
  free((*item)->resp);
  free(*item);
  *item = NULL;
}

void cacheResponse(priqueue_t* q, char* key, void* resp, int size)
{
  queue_item_t* item = (queue_item_t*)malloc(sizeof(queue_item_t));
  queue_item_t* tmp = NULL;
  item->req  = strndup(key, strlen(key));
  item->resp = resp;
  item->size = size;
  item->used = time(NULL);

  pthread_mutex_lock(&mutex);
  if(priqueue_insert(q, (void*)item) == -1) {
    tmp = priqueue_delete_min(q);
    if(tmp == NULL) { /* Cache size of 0? */
      destroyQueueItem(&item);
    } else { /* Destroy LRU item and insert new one */
      destroyQueueItem(&tmp);
      if(priqueue_insert(q, (void*)item) == -1) { /* Should never happen unless cache size == 0 */
        destroyQueueItem(&item);
      }
    }
  }
  pthread_mutex_unlock(&mutex);
}

void processHostRequest(char* inBuf, char* key, priqueue_t* q, int sockfd, int browserfd)
{
  const int toRead = 256;
  int bytesRead = 0;
  int read = -1;
  int inSize = strlen(inBuf);
  int bufSize = 256;
  int loop = 0;
  struct timeval timeo;
  char* buffer = (char*)malloc(bufSize*sizeof(char));
  fd_set readset;

  timeo.tv_sec  = 5; /* Wait up to 5 seconds for data */
  timeo.tv_usec = 0;

  /* Send in bite-sized chunks :) */
  loop = chunkSend(sockfd, inBuf, inSize);
  
  while(loop) {
    FD_ZERO(&readset);
    FD_SET(sockfd, &readset);
    select(sockfd+1, &readset, NULL, NULL, &timeo);
    if(FD_ISSET(sockfd, &readset)) {
      read = recv(sockfd, buffer+bytesRead, toRead, 0);
      if(read < 1)
        break;
      /* Back to client in bite-size chunks as we can get them */
      loop = chunkSend(browserfd, buffer+bytesRead, read);
      bytesRead += read;
      resizeBuffer(&buffer, &bufSize, toRead, bytesRead);
    } else { /* Is everything read? */
        break;
    }
  }

  buffer[bytesRead] = '\0';
  if(cacheHTTPResponse(buffer) && read > -1) {
    cacheResponse(q, key, (void*)buffer, bytesRead);
  } else {
    free(buffer);
  }
}

char* requestToKey(char* buffer)
{
  static const char startMatch[5] = { 'G', 'E', 'T', ' ', '\0' };
  static const char endMatch[2] = { '\n', '\0' };
  char* retval = NULL;
  char* start  = NULL;
  char* end    = NULL;

  if(buffer == NULL)
    return NULL;

  start = strstr(buffer, startMatch);

  if(start == NULL)
    return NULL;

  end = strstr(start, endMatch);

  /* Take the first line */
  retval = strndup(start, (end-start));
  return retval;
}

int checkCache(int sockfd, priqueue_t* q, char* key)
{
  queue_item_t* item = NULL;

  pthread_mutex_lock(&mutex);
  item = priqueue_element_exists(q, (void*)key);
  pthread_mutex_unlock(&mutex);
  if(item != NULL) {
    chunkSend(sockfd, item->resp, item->size);
    item->used = time(NULL);
    return 1;
  }

  return 0;
}

void* connection_handler(void *arg){
  tdata_t* data = (tdata_t*)arg;
  int sockfd = -1;
  int clientsock = 0;
  int * x = NULL;
  char* buffer = NULL;
  char* key = NULL;
  priqueue_t *queue = data->queue, *jobs = data->jobs;
  sem_t* semaphore = data->sem;

  while(!done) {
    sem_wait(semaphore);
    
    if(done)
      break;

    pthread_mutex_lock(&job_mutex);
    x = (int*)priqueue_delete_min(jobs);
    pthread_mutex_unlock(&job_mutex);

    if(x == NULL)
      continue;

    sockfd = *x;

    if(sockfd == -1) {
      free(x);
      continue;
    }

    /* Recv request from client */
    buffer = readFromClient(sockfd);
  
    if(buffer == NULL || done) {
      closeSocket(sockfd);
      if(buffer != NULL)
        free(buffer);
      free(x);
      if(!done)
        continue;
      break;
    }

    key = requestToKey(buffer);

    /* Check cache and serve if possible */
    if(checkCache(sockfd, queue, key) || done) {
      closeSocket(sockfd);
      free(key);
      free(buffer);
      free(x);
      if(!done)
        continue;
      break;
    }

    /* Parse request for host since not in cache */
    clientsock = initHostRequest(buffer);

    if(clientsock == -1 || done) {
      free(buffer);
      free(key);
      free(x);
      closeSocket(sockfd);
      if(!done)
        continue;
      break;
    }

    /* Actually get the data from the server */
    processHostRequest(buffer, key, data->queue, clientsock, sockfd);
  
    closeSocket(clientsock);
    closeSocket(sockfd);

    free(key);
    free(buffer);
    free(x);
  }

  return NULL;
}


int main(int argc, char *argv[]){
  int port;
  int serversock;
  int i = 0;
  int res = 0;
  int acceptsock = -1;
  int num_requests = 0;
  int* x = NULL;
  queue_item_t* tmp = NULL;
  struct sigaction sa;
  pthread_t threads[MAX_THREADS];
  priqueue_t queue, jobs;
  tdata_t data;
  pthread_attr_t attr;
  sem_t semaphore;

  /* Signal handling */
  sa.sa_handler = sigINT_handler;
  sa.sa_flags   = 0;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGPIPE, &sa, NULL);

  /* Basic error-checking */
  if(argc < 2) {
    fprintf(stderr, "Error: No port specified.\n\tUsage: ./proxy <port>\n");
    return 1;
  }

  port = atoi(argv[1]);

  if(port < 1000 || port > 30000) {
    fprintf(stderr, "Error: Port number %d out of range. Please use a port between 1000 and 30000\n", port);
    return 1;
  }

  /* Server setup */
  serversock = newServerSocket(argv[1]);

  if(serversock == -1) {
    fprintf(stderr, "Error: Could not bind/listen to port %d\n", port);
    return 1;
  }

  /* Attributes */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  /* Setup synchronization mutex */
  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_init(&hostcheck, NULL);
  sem_init(&semaphore, 0, 0);

  /* Implement queue for caching & jobs */
  priqueue_init(&queue, MAX_CACHE, comparer, match);
  priqueue_init(&jobs, MAX_PENDING, compare_jobs, match_jobs); /* Up to 500 waiting jobs */

  /* init threads */
  data.queue = &queue;
  data.jobs  = &jobs;
  data.sem   = &semaphore;

  for(i = 0 ; i < MAX_THREADS ; ++i)
    pthread_create(&threads[i], &attr, connection_handler, (void*)&data);

  /* The server loop */
  while(!done) {
    x = (int*)malloc(sizeof(int));
    
    if((acceptsock = acceptSocket(serversock)) == -1) { /* If an error occurs or interrupted */
      if(!done)
        fprintf(stderr, "Error: Failed to connect client.\n");
      free(x);
      continue;
    }
    
    *x = acceptsock;

    pthread_mutex_lock(&job_mutex);
    res = priqueue_insert(&jobs, (void*)x);
    pthread_mutex_unlock(&job_mutex);
    if(res == -1) {
      closeSocket(acceptsock);
      free(x); /* Lost (rejected) request */
    } else {
      sem_post(&semaphore);
    }

    acceptsock = -1;
    num_requests++;
  }
  closeSocket(serversock);

  for(i = 0 ; i < MAX_THREADS ; ++i) /* Make sure everything gets through */
    sem_post(&semaphore);

  for(i = 0 ; i < MAX_THREADS ; ++i)
    pthread_join(threads[i], NULL);

  printf("%10d total HTTP requests served.\n", num_requests);

  /* Cleanup queue elements */
  for(i = 0 ; i < MAX_CACHE ; ++i) {
    tmp = (queue_item_t*)priqueue_delete_min(&queue);
    if(tmp == NULL)
      break;
    destroyQueueItem(&tmp);
  }

  for(i = 0 ; i < MAX_PENDING ; ++i) {
    x = (int*)priqueue_delete_min(&jobs);
    if(x == NULL)
      break;
    free(x);
  }

  sem_destroy(&semaphore);
  pthread_mutex_destroy(&mutex);
  pthread_mutex_destroy(&job_mutex);
  pthread_mutex_destroy(&hostcheck);
  pthread_attr_destroy(&attr);
  priqueue_destroy(&queue);
  priqueue_destroy(&jobs);
	return 0;
}

