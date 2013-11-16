#include <stdio.h>

void main()
{
	int x;

	/*
	 * Use only the memory in x to print out "fun"
	 * in between this comment line and the next
	 * comment line.
	 */
    char * y = (char*)&x;
    y[0] = 'f';
    y[1] = 'u';
    y[2] = 'n';
    y[3] = '\0';


	/*
	 * You should not write any code below this point.
	 */
	printf("%s\n", &x);
}
