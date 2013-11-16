#include <stdio.h>

void main()
{
	int i = 10;

	while (--i > 0)
	{
		printf("%d...\n", i);
		sleep(1);
	}	
	printf("Blastoff!\n");
}

