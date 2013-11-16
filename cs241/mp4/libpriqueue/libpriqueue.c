/*
 * Machine Problem #4
 * CS 241
 * The University of Illinois
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"

/*
 * You can find the struct priqueue_t in libpriqueue.h.
 */


/*
 * priqueue_init()
 * Initializes the priqueue_t data structure.
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
  if(q == 0)
    return;
  q->head = 0;
  q->size = 0;
  q->comp = comparer;
}

/*
 * priqueue_offer()
 * Inserts the specified element into this priority queue.
 *
 * Returns: The zero-based index where ptr is stored in the priority queue,
 * where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
  int x = -1;
  leaf_t * curr = 0;
  leaf_t * tmp = 0;
  if(q == 0 || ptr == 0)
    return -1;

  tmp = (leaf_t*)malloc(sizeof(leaf_t));
  tmp->value = ptr;
  if(q->size == 0) {
    q->head = tmp;
    q->head->prev = 0;
    q->head->next = 0;
    q->size++;
    return 0;
  }

  curr = q->head;
  while(curr !=  0) {
    x++;
    if(q->comp(curr->value, ptr) >= 0) {
      tmp->next = curr;
      tmp->prev = curr->prev;
      if(q->head == curr)
        q->head = tmp;
      else
        curr->prev->next = tmp;
      curr->prev = tmp;
      break;
    } else if(curr->next == 0) {
      tmp->prev = curr;
      tmp->next = 0;
      curr->next = tmp;
      break;
    }
    curr = curr->next;
  }

  q->size++;

	return x;
}


/*
 * priqueue_peek()
 * Retrieves, but does not remove, the head of this queue, returning NULL if
 * this queue is empty.
 *
 * Returns: The head of the queue, or NULL if the queue is empty.
 */
void *priqueue_peek(priqueue_t *q)
{
	return (q == 0 || q->head == 0) ? 0 : q->head->value;
}

/* Helper function */
void* pq_remove(priqueue_t* q, leaf_t* node)
{
  if(node == 0 || q == 0)
    return 0;
  void* retval = 0;
  leaf_t * tmp = node;
  if(node == q->head) {
    q->head = q->head->next;
    if(q->head != 0) {
      q->head->prev = 0;
    }
  } else {
    node->prev->next = node->next;
    if(node->next != 0)
      node->next->prev = node->prev;
  }
  retval = node->value;
  q->size--;
  free(tmp);
  return retval;
}


/*
 * priqueue_poll()
 * Retrieves and removes the head of this queue, or NULL if this queue
 * is empty.
 *
 * Returns: The head of this queue, or NULL if this queue is empty.
 */
void *priqueue_poll(priqueue_t *q)
{
  if(q == 0 || q->size == 0)
    return 0;
  return pq_remove(q, q->head);
}


/*
 * priqueue_at()
 * Returns the element at the specified position in this list, or NULL if
 * the queue does not contain an index'th element.
 *
 * Returns: The index'th element in the queue, or NULL if the queue does
 * not contain in index'th element.
 */
void *priqueue_at(priqueue_t *q, int index)
{
  int i = 0;
  leaf_t* curr = 0;
  if(q == 0 || index >= q->size || index < 0)
    return 0;
  curr = q->head;
  for(i = 0; i < index; i++,curr=curr->next);
  return (curr == 0) ? 0 : curr->value;
}


/*
 * priqueue_remove()
 * Removes all instances of ptr from the queue. This function should not
 * use the comparer function, but check if the data contained in each
 * element of the queue is equal (==) to ptr.
 *
 * Returns: The number of entries removed.
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
  int x = 0;
  leaf_t* curr = 0;
  leaf_t* tmp = 0;
  if(q == 0)
    return 0;
  curr = q->head;
  while(curr != 0) {
    if(*((int*)curr->value) == *((int*)ptr) || *((char*)curr->value) == *((char*)ptr)) {
      tmp = curr;
      if(q->head == curr) {
        q->head = curr->next;
        q->head->prev = 0;
        curr = q->head;
      } else {
        if(curr->next != 0) {
          curr->next->prev = curr->prev;
        }
        curr->prev->next = curr->next;
        curr = curr->next;
      }
      free(tmp);
      tmp = 0;
      x++;
      q->size--;
      continue;
    }
    curr = curr->next;
  }
	return x;
}


/*
 * priqueue_remove_at()
 * Removes the specified index from the queue, moving later elements up
 * a spot in the queue to fill the gap.
 *
 * Returns: The element removed from the queue, or NULL if the specified
 * index does not exist.
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
  int i = 0;
  leaf_t * curr = 0;
  void * retval = 0;
  if(q == 0 || index >= q->size)
	  return 0;
  curr = q->head;
  for(i = 0 ; i < index ; curr=curr->next,i++);
  curr->prev->next = curr->next;
  retval = curr->value;
  free(curr);
  q->size--;
  return retval;
}


/*
 * priqueue_size()
 * Returns the number of elements in the queue.
 *
 * Returns: The number of elements in the queue.
 */
int priqueue_size(priqueue_t *q)
{
  if(q == 0)
    return 0;
	return q->size;
}

void pq_destroy(leaf_t* node)
{
  if(node == 0)
    return;
  if(node->next != 0)
    pq_destroy(node->next);
  free(node);
}

/*
 * priqueue_destroy()
 * Destroys and frees all the memory associated with q.
 */
void priqueue_destroy(priqueue_t *q)
{
  if(q == 0)
    return;
  pq_destroy(q->head);
}

