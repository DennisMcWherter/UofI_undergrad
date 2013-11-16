/*
 * Using the function pointer part of pthread_create()
 */

#include <pthread.h>
#include <stdio.h>

/*
 * int pthread_create(
 *     pthread_t *thread,
 *     const pthread_attr_t *attr,
 *     void *(*start_routine)(void*),    <-- What pattern does this describe?
 *     void *arg
 * );
 */

/* func() */
void main()
{
	pthread_t t;

	pthread_create(&t, NULL, func, NULL);
	printf("Thread launched!\n");

	pthread_join(t, NULL);
	printf("Thread joined back to main().\n");
}
