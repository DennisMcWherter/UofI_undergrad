#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

void queue_init(queue_t *q) {

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

void queue_destroy(queue_t *q) {
    while(queue_size(q) > 0) {
        queue_pop_front(q);
    }
}

void *queue_pop_front(queue_t *q) {
    struct queue_node *front;
    void *item;

    if(queue_size(q) == 0) {
        return NULL;
    }

    front = q->head;
    q->head = q->head->next;
    q->size--;

    item = front->item;
    free(front);

    if(queue_size(q) == 0) {
        // just cleaning up
        q->head = NULL;
        q->tail = NULL;
    }

    return item;
}


void *queue_at(queue_t *q, int pos){
    int i;
    struct queue_node *node;
    if(q == NULL)
    	return NULL;
    	

	for(i=0, node=q->head; i<pos && node != NULL; i++) node=node->next;
	if(i != pos)
		return 0;
	else
		return node->item;

}


void queue_push_back(queue_t *q, void *item) {
    struct queue_node *back = malloc(sizeof(struct queue_node));

    back->item = item;
    back->next = NULL;
    if(queue_size(q) == 0) {
        q->head = back;
    } else {
        q->tail->next = back;
    }
    q->tail = back;
    q->size++;
}

unsigned int queue_size(queue_t *q) {
    return q->size;
}

void queue_iterate(queue_t *q, void (*iter_func)(void *, void *), void *arg) {
    struct queue_node *node;
    if(queue_size(q) == 0) {
        return;
    }

    node = q->head;
    while(node != NULL) {
        iter_func(node->item, arg);
        node = node->next;
    }
}
