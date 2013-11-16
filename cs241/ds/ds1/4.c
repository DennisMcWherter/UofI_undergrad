#include <string.h>
#include <stdlib.h>

void main()
{
	/*
	 * Allocate enough memory to store 11 chars,
         * the ten letters and the NULL.
	 */
	char *s = malloc(11 * sizeof(char));
    memset(s, 0, sizeof(s));
	strcat(s, "Hello");
	strcat(s, "World");
    printf("%s\n", s);
}
