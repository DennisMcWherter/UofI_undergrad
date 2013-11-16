#ifndef _PRIQUEUE_H
#define _PRIQUEUE_H

typedef struct _priqueue {
    int MaxSize;
    int Size;
    void **Elements;
    int (*comparer)(const void*, const void*);
    int (*match)(const void*, const void*);
} priqueue_t;

void priqueue_init(priqueue_t* Q, int MaxElements, int (*comparer)(const void*, const void*), int (*match)(const void*, const void*));
void priqueue_destroy(priqueue_t* Q);
int priqueue_insert(priqueue_t* Q, void* X);
void* priqueue_element_remove(priqueue_t* Q, void* key);
void* priqueue_element_exists(priqueue_t* Q, void* key);
void* priqueue_delete_min(priqueue_t* Q);

#endif

