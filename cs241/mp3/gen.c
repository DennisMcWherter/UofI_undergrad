/*
Machine Problem #3
CS 241, Fall 2011
*/


/*Generates COUNT unique random integers in the range [0,RAND_MAX]
 * 
 *
 * Code based on example given in tsearch man pages
 * Stores generated integers in binary tree
 * Checks tree for uniqueness before printing
 */
 


#define _GNU_SOURCE     /* Expose declaration of tdestroy() */
#include <search.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void *root = NULL;

void print_usage(char *program_name)
{
	fprintf(stderr, "Generates a series of numbers, one number per line.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s <Count of numbers to generate>\n", program_name);
}

void *xmalloc(unsigned n){
   void *p;
   p = malloc(n);
   if (p)
       return p;
   fprintf(stderr, "insufficient memory\n");
   exit(EXIT_FAILURE);
}

int compare(const void *pa, const void *pb) {
   return *(int *) pa - *(int *) pb;
}

void blank(void* arg){}

int main(int argc, char*argv[]) {
   int i, *ptr;
   int count;
   void *val;

	/* Usage Check */
	if (argc != 2)
	{
		print_usage(argv[0]);
		return 1;
	}

	count = atoi(argv[1]);
	if (count <= 0)
	{
		print_usage(argv[0]);
		return 1;
	}

	if (count > RAND_MAX)
	{
		fprintf(stderr, "You requested more numbers than this program can generate.\n");
		return 1;
	}
	
   srand(time(NULL));
   ptr = xmalloc(sizeof(int)*count);
   for (i = 0; i < count; i++) {
      
       ptr[i] = rand();
       val = tsearch((void *) &ptr[i], &root, compare);
       if (val == NULL)
           exit(EXIT_FAILURE);
       else if ((*(int **) val) != &ptr[i]){
       		i--;
       }else{
       		printf("%d\n",ptr[i]);
       	}
   }
   tdestroy(root, blank);
   free(ptr);
   exit(EXIT_SUCCESS);
}

