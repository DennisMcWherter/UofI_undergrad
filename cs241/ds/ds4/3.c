/*
 * Managing shared memory in threads and using pthread_join
 */

#include <pthread.h>
#include <stdio.h>

int x;
const int TOTAL_RUNS = 1000000;

void *up(void *ptr)
{
	int i;
	for (i = 0; i < TOTAL_RUNS; i++)
		x++;

	printf("Finished adding 1 a total of %d times.\n", TOTAL_RUNS);
}

void *down(void *ptr)
{
	int i;
	for (i = 0; i < TOTAL_RUNS; i++)
		x--;

	printf("Finished subtracting 1 a total of %d times.\n", TOTAL_RUNS);
}

void main()
{
	pthread_t t1, t2;
	
	pthread_create(&t1, NULL, up, NULL);
	pthread_create(&t2, NULL, down, NULL);

	printf("Ending value: %d\n", x);
}
