#include <stdio.h>
#include <stdlib.h>
// #define NUMCOLS 8000 
// #define NUMROWS 8000
#define NUMCOLS (1<<13)  // 8192
#define NUMROWS (1<<13)  // 8192


int
main(int argc, char **argv) {

  // heap allocating because they are too big to stack allocate
  int *src = (int *)malloc(NUMCOLS * NUMROWS * sizeof(int));  
  int *dest = (int *)malloc(NUMROWS * NUMCOLS * sizeof(int));  

  // initialize to make sure the memory has been paged in.
  for (int i = 0 ; i < NUMCOLS*NUMROWS ; i ++) {
	 src[i] = i;
	 dest[i] = 0;
  }

  // dest and src represent two-dimensional arrays 
  for (int i = 0 ; i < NUMCOLS ; i ++) { 
	 for (int j = 0 ; j < NUMROWS ; j ++) { 
		dest[i*NUMROWS + j] = src[j*NUMCOLS + i]; 
	 } 
  } 

  
#if 0
  // set the 0 to 1 to verify that your new version is correct.
  for (int i = 0 ; i < NUMCOLS ; i ++) { 
	 for (int j = 0 ; j < NUMROWS ; j ++) { 
		if (dest[i*NUMROWS + j] != src[j*NUMROWS + i]) 
		  printf("mismatch at (%d, %d) = [%d, %d]\n", 
					i, j, dest[i*NUMROWS + j], src[j*NUMROWS + i]);
	 } 
  } 
#endif

  printf("%d\n", dest[random() % (NUMCOLS*NUMROWS)]);
}

