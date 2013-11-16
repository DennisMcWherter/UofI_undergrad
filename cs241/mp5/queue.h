#ifndef __QUEUE_H__
#define __QUEUE_H__

struct queue_node {
	void *item;
	struct queue_node *next;
};

typedef struct _queue {
	struct queue_node *head;
	struct queue_node *tail;
	unsigned int size;
} queue_t;

void queue_init(queue_t *q);
void queue_destroy(queue_t *q);

void *queue_pop_front(queue_t *q);
void *queue_at(queue_t *q, int pos);
void queue_push_back(queue_t *q, void *item);
unsigned int queue_size(queue_t *q);

void queue_iterate(queue_t *q, void (*iter_func)(void *, void *), void *arg);

#endif
