/*
Machine Problem #5
CS 241
*/


#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "parmake.h"
#include "parser.h"
#include "queue.h"

static int threads  = 0;
static int sleeping = 0;

queue_t g_q;
queue_t g_p;

pthread_mutex_t accessQueue;
pthread_mutex_t accessProced;
pthread_mutex_t killing;
pthread_mutex_t waking;
pthread_cond_t wakeupMain;
pthread_cond_t killThreads;

void debug_view()
{
  unsigned i = 0;
  int j = 0;
  parsed_data_t * parsed = 0;

  for(i = 0; i < queue_size(&g_q); ++i) {
    parsed = (parsed_data_t*)queue_at(&g_q, i);
    if(parsed == NULL)
      break;
    printf("Key: %s\n\tnumCmds: %d\n\tnumDeps: %d\n\tCommands:", parsed->key, parsed->numCmds, parsed->numDeps);
    for(j = 0; j < parsed->numCmds; ++j)
      printf(" %s", parsed->commands[j]);
    printf("\n\tDeps:");
    for(j = 0; j < parsed->numDeps; ++j)
      printf(" %s", parsed->deps[j]);
    printf("\n");
  }
}

int checkDep(char* dep) {
  unsigned i = 0;
  parsed_data_t* parsed = NULL;
  
  for(i = 0 ; i < queue_size(&g_p); ++i) {
    parsed = (parsed_data_t*)queue_at(&g_p, i);
    if(strncmp(dep, parsed->key, strlen(dep)) == 0)
       return 0;
  }
  return 1;
}

void* processWork(void* data)
{
  int jobs = *(int*)data;
  int proc = 0;
  int i = 0;
  int b = 0;
  parsed_data_t* parsed = NULL;
  
  while(1) {
    /* Grab next available element from Queue */
    pthread_mutex_lock(&accessQueue);
    parsed = (parsed_data_t*)queue_pop_front(&g_q);
    pthread_mutex_unlock(&accessQueue);
    
    /* Determine whether its dependencies have been met and it's non-NULL */
    if(parsed != NULL) {
      pthread_mutex_lock(&accessProced);
      for(i = 0; i < parsed->numDeps; ++i) {
        /* If deps not met, send to back of queue and return to top of loop */
        if(checkDep(parsed->deps[i])) {
          pthread_mutex_lock(&accessQueue);
          queue_push_back(&g_q, (void*)parsed);
          pthread_mutex_unlock(&accessQueue);
          b = 1;
          break;
        }
      }
      pthread_mutex_unlock(&accessProced);
      if(b) {
        b = 0;
        continue;
      }
    } else { /* If NULL, check processed against the number of threads */
      /* If processed >= threads, then sleep this thread and wait on signal */
      pthread_mutex_lock(&accessProced);
      proc = queue_size(&g_p);
      pthread_mutex_unlock(&accessProced);
      if(proc >= threads || threads > jobs) {
        pthread_mutex_lock(&waking);
        sleeping++;
        pthread_cond_signal(&wakeupMain);
        pthread_mutex_unlock(&waking);
      
        pthread_mutex_lock(&killing);
        pthread_cond_wait(&killThreads, &killing);
        pthread_mutex_unlock(&killing);
        break;
      }
      continue;
    }

    /* If deps met, then run its commands */
    for(i = 0; i < parsed->numCmds; ++i)
      system(parsed->commands[i]);
    
    /* Add to the processed queue */
    pthread_mutex_lock(&accessProced);
    queue_push_back(&g_p, (void*)parsed);
    proc = queue_size(&g_p);
    pthread_mutex_unlock(&accessProced);
    
    /* If the size of the processed queue == jobs, signal main */
    if(proc >= jobs) {
      pthread_mutex_lock(&waking);
      sleeping++;
      pthread_cond_signal(&wakeupMain);
      pthread_mutex_unlock(&waking);
      
      pthread_mutex_lock(&killing);
      pthread_cond_wait(&killThreads, &killing);
      pthread_mutex_unlock(&killing);
      break;
    }
  }
  return 0;
}

int main(int argc, char **argv) {
  int i = 0;
  int jobs = 0;
  pthread_t* thread = NULL;

  if(argc != 3) {
    fprintf(stderr, "Invalid usage.\nUsage: ./parmake -j# <make_file>\n");
    return -1;
  }

  argv[1] += 2; /* Advance to integer */
  threads = atoi(argv[1]);
  if(threads < 1) {
    fprintf(stderr, "Number of threads must be greater than 1.\n");
    return -1;
  }

  pthread_mutex_init(&accessQueue, NULL);
  pthread_mutex_init(&accessProced, NULL);
  pthread_mutex_init(&killing, NULL);
  pthread_mutex_init(&waking, NULL);
  pthread_cond_init(&wakeupMain, NULL);
  pthread_cond_init(&killThreads, NULL);
  queue_init(&g_q);
  queue_init(&g_p);

  parser_parse_makefile(argv[2], parsed_command, parsed_new_key, parsed_dependency);

  //debug_view(); /* Temporary method to see how stuff is stored */

  jobs = queue_size(&g_q);
  
  thread = (pthread_t*)malloc(threads*sizeof(pthread_t));

  for(i = 0 ; i < threads; ++i)
    pthread_create(&thread[i], 0, processWork, (void*)&jobs);
  
  /* Wait for everything to signal back */
  pthread_mutex_lock(&waking);
  while(sleeping != threads)
    pthread_cond_wait(&wakeupMain, &waking);
  pthread_mutex_unlock(&waking);

  pthread_mutex_lock(&killing);
  pthread_cond_broadcast(&killThreads);
  pthread_mutex_unlock(&killing);

  for(i = 0; i < threads; ++i)
    pthread_join(thread[i], (void*)0);

  /* All cleanup */
  free(thread);
  cleanup_this();
  pthread_mutex_destroy(&accessQueue);
  pthread_mutex_destroy(&accessProced);
  pthread_mutex_destroy(&killing);
  pthread_mutex_destroy(&waking);
  pthread_cond_destroy(&wakeupMain);
  pthread_cond_destroy(&killThreads);
  queue_destroy(&g_q);
  queue_destroy(&g_p);

  return 0; 
}

/* parsed data functions */
void parsed_data_init(parsed_data_t* data)
{
  if(data == NULL)
    return;

  data->numCmds  = 0;
  data->cmdCnt   = 16;
  data->commands = (char**)malloc(16*sizeof(char*));

  data->numDeps  = 0;
  data->depsCnt  = 16;
  data->deps     = (char**)malloc(16*sizeof(char*));
}

void parsed_data_destroy(parsed_data_t* data)
{
  int i = 0;
  if(data == NULL)
    return;

  for(i = 0; i < data->numCmds; ++i)
    free(data->commands[i]);

  for(i = 0; i < data->numDeps; ++i)
    free(data->deps[i]);

  free(data->deps);
  free(data->commands);
  free(data->key);
}

/* Cleanup */
void cleanup_this()
{
  parsed_data_t* parsed = NULL;
  
  while((parsed = (parsed_data_t*)queue_pop_front(&g_q))) {
    parsed_data_destroy(parsed);
    free(parsed);
  }
  
  while((parsed = (parsed_data_t*)queue_pop_front(&g_p))) {
    parsed_data_destroy(parsed);
    free(parsed);
  }
}

/* Callbacks */
void parsed_command(const char* a, const char* b)
{
  parsed_append(a, b, append_cmd);
}

void parsed_new_key(const char* a)
{
  parsed_data_t* parsed = (parsed_data_t*)malloc(sizeof(parsed_data_t));
  parsed_data_init(parsed);
  parsed->key = strndup(a, strlen(a));
  queue_push_back(&g_q, (void*)parsed);
}

void parsed_dependency(const char* a, const char* b)
{
  parsed_append(a, b, append_dep);
}

void parsed_append(const char* a, const char* b, void(*parse)(const char*,parsed_data_t*))
{
  unsigned i = 0;
  parsed_data_t* parsed = NULL;

  for(i = 0; i < queue_size(&g_q); ++i) {
    parsed = (parsed_data_t*)queue_at(&g_q, i);
    if(parsed == NULL)
      break;
    if(strncmp(parsed->key, a, strlen(a)) == 0)
      break;
    parsed = NULL;
  }

  if(parsed == NULL)
    return;

  parse(b, parsed);
}

void append_data(const char* a, int* num, int* cnt, char*** arr) {
  if((*num) == (*cnt)) {
    (*cnt) *= 2;
    (*arr) = (char**)realloc((*arr), (*cnt)*sizeof(char*));
  }

  (*arr)[(*num)] = strndup(a, strlen(a));
  (*num)++;
}

void append_dep(const char* a, parsed_data_t* parsed)
{
  append_data(a, &parsed->numDeps, &parsed->depsCnt, &parsed->deps);
}

void append_cmd(const char* a, parsed_data_t* parsed)
{
  append_data(a, &parsed->numCmds, &parsed->cmdCnt, &parsed->commands);
}

