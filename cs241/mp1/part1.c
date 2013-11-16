/*
 * Machine Problem #1
 * CS 241
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "mp1-functions.h"

int main()
{
	/*
	 * [Part 1]:
	 *   Edit the provided part1.c file to print out the ten "Illinois" lines
	 *   that are provided in the code inside mp1-functions.c.
	 */
    int i = 0;
    int ** j = 0;
    int k = 132;
    char x[4];
    void * v = 0;

    j = (int**)malloc(4*sizeof(int*));
    for(i = 0; i < 4; ++i)
        j[i] = (int*)malloc(sizeof(int));

    *j[0] = 8942;
    *j[3] = 0;
    x[3]  = 'u';
    x[2]  = x[3] - 8;
    x[1]  = x[2] - 8;

    first_step(81);
    second_step(&k);
    double_step(j);
    strange_step(0);
    empty_step((void*)*j);
    two_step((void*)x, (char*)x);
    three_step((char*)0x6, (char*)0x8, (char*)0xA);
    step_step_step(x, x, x);

    x[0] = 1;
    it_may_be_odd(x, 1);

    v = malloc(sizeof(int));
    int * z = (int*)v;
    *z = 0;

    /* Big endian: shift 11 + 1 = divisible by 3
      Add 1 since char is only a byte */
    *z |= 0x01;
    *z <<= 0x0B;
    *z |= 0x01;

    the_end(v, v);

    for(i = 0; i < 4; ++i)
        free(j[i]);
    free(j);
    free(v);

	return 0;
}
