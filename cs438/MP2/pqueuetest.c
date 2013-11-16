/**
 * pqueuetest.c
 */

#include <stdio.h>
#include "priqueue.h"

int f(void* d1, void* d2)
{
  int v1, v2;

  if(d1 == NULL || d2 == NULL)
    return 0;

  v1 = *((int*)d1);
  v2 = *((int*)d2);

  return (v1 > v2) ? 1 : 0;
}

int main()
{
  priqueue_t q;
  int i = 0;
  int x[] = { 2, 1, 5, 7, 15, -23, 234, -1000, 4, 8, 10, 8 };
  void* val = NULL;

  init_priqueue(&q, &f);

  for(i = 0 ; i < sizeof(x) / sizeof(int) ; ++i) {
    printf("Pushing: %d\n", x[i]);
    priqueue_push(&q, &x[i]);
  }

  while((val = priqueue_pop(&q)) != NULL)
    printf("Popped: %d\n", *((int*)val));

  destroy_priqueue(&q);
  return 0;
}

