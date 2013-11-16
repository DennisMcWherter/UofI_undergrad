#include <stdio.h>
#include <stdlib.h>

#include "priqueue.h"

void priqueue_init(priqueue_t* Q, int MaxElements, int (*func1)(const void*, const void*), int (*func2)(const void*, const void*)) {
	Q->MaxSize = MaxElements;
	Q->Size = 0;
	Q->Elements = malloc((MaxElements + 1) * sizeof(void*));
	Q->Elements[0] = NULL;
	Q->comparer = func1;
	Q->match = func2;
}

int priqueue_insert(priqueue_t* Q, void* elem) {
    	int i;

    	if (Q->Size == Q->MaxSize) return -1;
    	for (i = ++Q->Size; (i/2 > 0 && Q->comparer(Q->Elements[ i / 2 ], elem) > 0); i /= 2)
    		Q->Elements[ i ] = Q->Elements[ i / 2 ];
    	
    	Q->Elements[ i ] = elem;
    	return 0;
}

void* priqueue_element_exists(priqueue_t* Q, void* key){
    	int i;

    	if (Q->Size == 0) return NULL;
    	for (i = 1; i <= Q->Size; i++)
    		if(Q->match(Q->Elements[ i ], key) == 0) return Q->Elements[ i ];
    	
    	return NULL;

}

void* priqueue_element_remove(priqueue_t* Q, void* key){
    	int i, Child;
    	void *Element=NULL, *LastElement;

    	if (Q->Size == 0) return NULL;
    	for (i = 1; i <= Q->Size; i++)
    		if(Q->match(Q->Elements[ i ], key) == 0) {
			Element = Q->Elements[ i ];
			break;
		}
    	
	if(i > Q->Size) return NULL;

    	LastElement = Q->Elements[ Q->Size-- ];
    	for (; i * 2 <= Q->Size; i = Child) {
        	/* Find smaller child */
        	Child = i * 2;
        	if (Child != Q->Size && Q->comparer(Q->Elements[ Child + 1 ], Q->Elements[ Child ]) < 0)
            		Child++;

        	/* Percolate one level */
        	if (Q->comparer(LastElement, Q->Elements[ Child ]) > 0)
            		Q->Elements[ i ] = Q->Elements[ Child ];
        	else
            		break;
    	}
    	Q->Elements[ i ] = LastElement;


    	return Element;

}

void* priqueue_delete_min(priqueue_t* Q) {
    	int i, Child;
    	void *MinElement, *LastElement;

	if (Q->Size == 0) return NULL;

    	MinElement = Q->Elements[ 1 ];
    	LastElement = Q->Elements[ Q->Size-- ];

    	for (i = 1; i * 2 <= Q->Size; i = Child) {
        	/* Find smaller child */
        	Child = i * 2;
        	if (Child != Q->Size && Q->comparer(Q->Elements[ Child + 1 ], Q->Elements[ Child ]) < 0)
            		Child++;

        	/* Percolate one level */
        	if (Q->comparer(LastElement, Q->Elements[ Child ]) > 0)
            		Q->Elements[ i ] = Q->Elements[ Child ];
        	else
            		break;
    	}
    	Q->Elements[ i ] = LastElement;

   	return MinElement;
}

void priqueue_destroy(priqueue_t* Q) {
    free(Q->Elements);
}


