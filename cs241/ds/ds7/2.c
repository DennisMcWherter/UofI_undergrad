#include <pthread.h>
#include <stdio.h>

/* Semaphore */
void my_sem_wait()
{
}

void my_sem_post()
{
}

/* Tester */
int x = 0;
const int TOTAL_RUNS = 1000000;

void *up(void *ptr)
{
	int i;
	for (i = 0; i < TOTAL_RUNS; i++)
	{
		my_sem_wait();
		x++;
		my_sem_post();
	}

	printf("Finished adding 1 a total of %d times.\n", TOTAL_RUNS);
}

void *down(void *ptr)
{
	int i;
	for (i = 0; i < TOTAL_RUNS; i++)
	{
		my_sem_wait();
		x--;
		my_sem_post();
	}

	printf("Finished subtracting 1 a total of %d times.\n", TOTAL_RUNS);
}


/* main() */
void main()
{
	/* Tester */
	pthread_t t1, t2;

	pthread_create(&t1, NULL, up, NULL);
	pthread_create(&t2, NULL, down, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("Ending value: %d\n", x);
}
