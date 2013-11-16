/*
 * Using threads for interactive programming
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>

double pi;

void *approx_pi(void *ptr)
{
	unsigned long success = 0;
	unsigned long tries = 0;

	while (1)
	{
		double x = rand() / (double)INT_MAX;
		double y = rand() / (double)INT_MAX;

		double r = sqrt((x * x) + (y * y));
		if (r < 1) success++;
		tries++;

		pi = (success / (double)tries) * 4;
	}
}

void main()
{
	pthread_t t;
	pthread_create(&t, NULL, approx_pi, NULL);
	
	char *s = malloc(100);

	while ((s = fgets(s, 100, stdin)) != NULL && strncmp(s, "exit", 4) != 0)
		printf("Pi: %f\n", pi);
}
