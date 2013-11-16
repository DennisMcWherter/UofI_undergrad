/**
 * mp1client.c
 *
 * MP1 Client
 *
 * Author: Dennis J. McWherter, Jr.
 */

#include "mp1.h"
#include "mp1util.h"
#include "ConcurrentQueue.h"

#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define NUM_THREADS 2

// Forward declarations
void* readThread(void* data);
void* writeThread(void* data);

int main(int argc, char* argv[])
{
  if(argc < 4) {
    fprintf(stderr, "Usage: %s <server> <port #> <output_file>\n", argv[0]);
    return -1;
  }

  int sockfd = initConnection(argv[1], argv[2]);

  if(sockfd == -1) {
    fprintf(stderr, "Fatal Error: Could not connect to server (%s:%s).\n", argv[1], argv[2]);
    return -1;
  }

  FILE* fp = fopen(argv[3], "w");

  if(fp == NULL) {
    close(sockfd);
    fprintf(stderr, "Fatal Error: Could not open file (%s).\n", argv[3]);
    return -1;
  }

  queue_t queue;
  init_queue(&queue);

  pthread_t threads[NUM_THREADS];

  thread_data_t sockdata = { &queue, { sockfd } };
  thread_data_t filedata = { &queue, { 0 } };
  filedata.data.fp = fp;

  // TODO: Should we error check pthreads?
  pthread_create(&threads[0], NULL, readThread, &sockdata);
  pthread_create(&threads[1], NULL, writeThread, &filedata);

  int i = 0;
  for(i = 0 ; i < NUM_THREADS ; ++i)
    pthread_join(threads[i], NULL);

  fclose(fp);
  close(sockfd);
  destroy_queue(&queue);

  return 0;
}


/**
 * Routine for the reading worker thread
 */
void* readThread(void* data)
{
  thread_data_t* tdata = (thread_data_t*)data;
  queue_t* q = tdata->queue;
  int sockfd = tdata->data.sockfd;
  int read = 0;
  int totalRead = 0;
  char frame[FRAME_SIZE];

  while((read = MP1_read(sockfd, &frame[totalRead], FRAME_SIZE - totalRead)) != 0) {
    totalRead += read;
    if(totalRead == FRAME_SIZE) {
      // Flip the frame from network byte order to host order
      short length = ntohs(*((short*)frame));
      unsigned position = ntohl(*((unsigned*)&frame[2]));

      // Pack data for queue
      data_entry_t entry;
      initDataEntry(&entry, &frame[6], length, position);

      // Only use packet if we pass CRC-12 check
      if(crc12(frame) == 0) {
        push_queue(q, &entry, sizeof(entry));
      }

      totalRead = 0; // Next frame
    }
  }

  data_entry_t done;
  done.data = NULL;
  done.length = done.position = 0;
  push_queue(q, &done, sizeof(done));

  return NULL;
}

/**
 * Routine for the writing worker thread
 */
void* writeThread(void* data)
{
  thread_data_t* tdata = (thread_data_t*)data;
  queue_t* q = tdata->queue;
  FILE* fp = tdata->data.fp;
  int end = 0;
  data_entry_t* entry = NULL;

  while(!end) {
    entry = pop_queue(q);
    while(entry == NULL) {
      waitForQueue(q);
      entry = pop_queue(q);
    }
    char* pdata = entry->data;
    if(pdata == NULL) {
      end = 1;
    } else {
      fseek(fp, entry->position, SEEK_SET);
      int offset = 0;
      while(offset != entry->length) {
        offset += MP1_fwrite(entry->data + offset, 1, entry->length - offset, fp); 
      }
      destroyDataEntry(entry);
      free(entry);
    }
  }

  // This should be the sentinel entry
  free(entry);

  return NULL;
}

