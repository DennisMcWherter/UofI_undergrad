/**
 * ConcurrentQueue.h
 *
 * Concurrent queue interface
 *
 * Author: Dennis J. McWherter, Jr.
 */

#ifndef CONCURRENTQUEUE_H__
#define CONCURRENTQUEUE_H__

#include <pthread.h>

/**
 * Queue item
 */
typedef struct _entry_t
{
  void* data;
  struct _entry_t* next;
} entry_t;

/**
 * Queue structure
 */
typedef struct _queue_t
{
  entry_t* front;
  entry_t* back;
  pthread_mutex_t lock;
  pthread_cond_t ready;
} queue_t;

// Functions
void init_queue(queue_t* queue);
void destroy_queue(queue_t* queue);

int isEmpty(queue_t* queue);
void waitForQueue(queue_t* queue);

void push_queue(queue_t* queue, const void* data, size_t length);
void* pop_queue(queue_t* queue);

#endif /** CONCURRENTQUEUE_H__ */

