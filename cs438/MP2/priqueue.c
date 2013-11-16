/**
 * priqueue.c
 *
 * Priority queue implementation
 *
 * @author Dennis J. McWherter, Jr.
 */

#include <stdlib.h>
#include "priqueue.h"

/**
 * Initialize the priority queue
 *
 * @param q   The queue object
 * @param f   The comparator function to use
 */
void init_priqueue(priqueue_t* q, pqueue_compare f)
{
  if(q == NULL || f == NULL)
    return;
  q->comparator = f;
  q->head = NULL;
  q->tail = NULL;
}

/**
 * Destroy the priority queue
 *
 * @param q   Queue to destroy
 */
void destroy_priqueue(priqueue_t* q)
{
  pqueue_item_t* next = NULL;
  pqueue_item_t* tmp = NULL;

  if(q == NULL)
    return;

  // We will free what we created, the data inside is not our responsibility
  next = q->head;
  while(next != NULL) {
    tmp = next;
    next = next->next;
    free(tmp);
  }
}

/**
 * Push an element onto the priority queue
 *
 * @param q     Queue to push the element on to
 * @param data  Data element to push on
 */
void priqueue_push(priqueue_t* q, void* data)
{
  pqueue_item_t* item = NULL;
  pqueue_item_t* next = NULL;
  pqueue_compare func = NULL;

  if(q == NULL || data == NULL)
    return;

  func = q->comparator;

  item = malloc(sizeof(pqueue_item_t));
  item->next = NULL;
  item->prev = NULL;
  item->data = data;
  next = q->head;

  if(next == NULL) { // Insert at the head of an empty list
    q->head = item;
    q->tail = item;
  } else {
    while(next != NULL && (*func)(next->data, data))
      next = next->next;
    if(next != NULL) { // Insert in the middle of the list or at head
      if(next == q->head)
        q->head = item;
      item->next = next;
      item->prev = next->prev;
      next->prev = item;
      if(item->prev != NULL)
        item->prev->next = item;
    } else { // Insert at the tail
      next = q->tail;
      q->tail = item;
      item->prev = next;
      next->next = item;
    }
  }
}

/**
 * Pop the highest priority element off the queue
 *
 * @param q   Queue to pop from
 * @return  The data of the highest priority element
 */
void* priqueue_pop(priqueue_t* q)
{
  void* data = NULL;
  pqueue_item_t* top = NULL;

  if(q == NULL)
    return NULL;

  if(q->head == q->tail)
    q->tail = NULL;

  top = q->head;
  if(top != NULL) {
    q->head = top->next;
    if(q->head != NULL)
      q->head->prev = NULL;
    data = top->data;
  }

  free(top);
  
  return data;
}

