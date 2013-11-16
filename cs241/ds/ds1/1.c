#include <stdlib.h>

void main()
{
	int i = 0;
	double *d = (double*)malloc(20*sizeof(double));

	for (i = 0; i < 20; i++)
		d[i] = i * 100;

    free(d);
}
