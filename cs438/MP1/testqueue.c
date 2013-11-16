/**
 * testqueue.c
 */

#include "ConcurrentQueue.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_THREADS 10

void* popThread(void* data)
{
  queue_t* q = (queue_t*)data;
  waitForQueue(q);
  unsigned* val = NULL;
  while(val == NULL) {
    waitForQueue(q);
    val = (unsigned*)pop_queue(q);
  }
  printf("%u\n", *val);
  free(val);
  pthread_exit(0);
}

void* pushThread(void* data)
{
  queue_t* q = (queue_t*)data;
  pthread_t self = pthread_self();
  push_queue(q, &self, sizeof(self));
  pthread_exit(0);
}

int main()
{
  pthread_t threads[NUM_THREADS];
  queue_t q;
  init_queue(&q);

  unsigned something = 523;
  push_queue(&q, &something, sizeof(something));

  int i = 0;
  for(i = 0 ; i < NUM_THREADS/2 ; ++i)
    pthread_create(&threads[i], NULL, popThread, (void*)&q);

  for(i = NUM_THREADS/2 ; i < NUM_THREADS ; ++i)
    pthread_create(&threads[i], NULL, pushThread, &q);

  for(i = 0 ; i < NUM_THREADS ; ++i)
    pthread_join(threads[i], NULL);

  destroy_queue(&q);
  return 0;
}

