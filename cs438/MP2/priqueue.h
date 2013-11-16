/**
 * priqueue.h
 *
 * Simple priority queue
 * NOTE: This priority queue does NOT make a deep copy of any data
 *  Therefore, if the data is supposed to persist, it is the responsibility
 *  of the user to make a copy before adding it to the queue. Moreover,
 *  it is also the responsibility of the user to free all data inserted
 *  into the structure. destroy_priqueue() will manage all internal memory
 *
 * @author Dennis J. McWherter, Jr.
 */

#ifndef PRIQUEUE_H__
#define PRIQUEUE_H__

typedef int(*pqueue_compare)(void*,void*);

/**
 * Item structure
 */
typedef struct _pqueue_item
{
  void* data;
  struct _pqueue_item* prev;
  struct _pqueue_item* next;
} pqueue_item_t;

/**
 * Priority queue structure
 */
typedef struct
{
  pqueue_compare comparator;
  pqueue_item_t* head;
  pqueue_item_t* tail;
} priqueue_t;


// Methods
void init_priqueue(priqueue_t* q, pqueue_compare f);
void destroy_priqueue(priqueue_t* q);
void priqueue_push(priqueue_t* q, void* data);
void* priqueue_pop(priqueue_t* q);

#endif /** PRIQUEUE_H__ */

