/**
 * ConcurrentQueue.c
 *
 * Concurrent queue implementation
 *
 * Author: Dennis J. McWherter, Jr.
 */

#include "ConcurrentQueue.h"

#include <stdlib.h>
#include <string.h>

/**
 * Initialize a queue
 *
 * NOTE: Input must be valid
 *
 * @param queue   The queue to initialize
 */
void init_queue(queue_t* queue)
{
  queue->front = NULL;
  queue->back  = NULL;
  pthread_mutex_init(&queue->lock, NULL);
  pthread_cond_init(&queue->ready, NULL);
}

/**
 * Destroy a valid queue
 */
void destroy_queue(queue_t* queue)
{
  entry_t* data = queue->front;
  entry_t* tmp = NULL;

  while(data != NULL) {
    tmp = data;
    data = data->next;
    free(tmp->data);
    free(tmp);
  }

  pthread_mutex_destroy(&queue->lock);
  pthread_cond_destroy(&queue->ready);
}

/**
 * Check if queue is empty or not
 *
 * @param queue   Queue to check
 *
 * @return 0 if empty, 1 otherwise. -1 on error.
 */
int isEmpty(queue_t* queue)
{
  if(queue == NULL)
    return -1;

  pthread_mutex_lock(&queue->lock);
  int ret = (queue->front == NULL) ? 0 : 1;
  pthread_mutex_unlock(&queue->lock);

  return ret;
}

/**
 * Wait for the queue to be ready
 *
 * This call will BLOCK until the queue
 * is ready.
 */
void waitForQueue(queue_t* queue)
{
  if(queue == NULL)
    return;

  pthread_mutex_lock(&queue->lock);
  int ready = (queue->front == NULL) ? 0 : 1;
  if(!ready)
    pthread_cond_wait(&queue->ready, &queue->lock);
  pthread_mutex_unlock(&queue->lock);
}

/**
 * Push an element to the back of the queue
 *
 * @param queue   Queue to push item to
 * @param data    Data to add
 * @param length  Data length
 */
void push_queue(queue_t* queue, const void* data, size_t length)
{
  if(queue == NULL)
    return;

  entry_t* entry = malloc(sizeof(entry_t));
  void* copy = malloc(length);
  
  memcpy(copy, data, length);

  entry->data = copy;
  entry->next = NULL;

  pthread_mutex_lock(&queue->lock);
  
  if(queue->front == NULL)
    queue->front = entry;
  if(queue->back != NULL)
    queue->back->next = entry;
  queue->back = entry;

  pthread_cond_broadcast(&queue->ready);
  pthread_mutex_unlock(&queue->lock);
}

/**
 * Retrieve the front element from the queue
 *
 * @param queue   Queue to pop
 *
 * @return  Top element data from the queue. NULL if queue is invalid queue.
 *
 * NOTE: The user is responsible for freeing the memory returned.
 *  Also, this method will block on an empty queue.
 */
void* pop_queue(queue_t* queue)
{
  if(queue == NULL)
    return NULL;

  pthread_mutex_lock(&queue->lock);
  entry_t* entry = queue->front;
  void* data = NULL;

  if(entry != NULL) {
    if(queue->front == queue->back)
      queue->back = NULL;
    queue->front = entry->next;
    data = entry->data;
    free(entry);
  }

  pthread_mutex_unlock(&queue->lock);

  return data;
}

