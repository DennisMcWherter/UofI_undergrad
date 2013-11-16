#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *  1. Create a few different functions to manipulate a single
 *     char as part of a string.  Make sure the function is of
 *     the format:
 *         void _____(char *____) {  ...  }
 */




void x(char* y) {
  *y++;
}






void manip(char *s, void (*func)(char *))
{
	int i;
	for (i = 0; i < strlen(s); i++)
	{
		/* How do we call the function we created? */
	}
}

int main()
{
	char *s = malloc(100);
	strcpy(s, "The quick brown fox jumps over the lazy dog.");

	/* How are we going to call manip()? */

	free(s);
	return 0;
}
