#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

typedef float v4sf __attribute__ ((vector_size (4*sizeof(float))));

#define	SIZE	32
#define	ITER	1000000

// Matrix-Vector multiplication
// mat is a SIZE by SIZE matrix, that is arranged in row-column, format,
// That is, you first select a particular row, and then a particular column.
// Each row is laid out as a one-dimensional, array, so if you wanted
// to select a particular row, you would use mat[row].  You can
// also select smaller intervals, by using &mat[row][col].
// The vector is also laid out as a one-dimensional arrow, similar to a row.
// M-V multiplication proceeds by taking the dot product of a matrix row
// with the vector, and doing this for each row in the matrix
float *mv_mult(float mat[SIZE][SIZE], float vec[SIZE]) {
	static float ret[SIZE];
	float tempArr[4];
  int i, j;
  v4sf acc, tmp, tmp2, tmpAcc;
  
  acc = __builtin_ia32_xorps(acc, acc);

	for (i = 0; i < SIZE; i++) {
		ret[i] = 0;
    for (j = 0; j < SIZE-3; j+=4) {
		//	ret[i] += mat[i][j] * vec[j];
      tmp  = __builtin_ia32_loadups(&vec[j]);
      tmp2 = __builtin_ia32_loadups(&mat[i][j]);
      acc  = __builtin_ia32_addps(acc, __builtin_ia32_mulps(tmp, tmp2));
    }
    __builtin_ia32_storeups(tempArr, acc);
    ret[i] = tempArr[0] + tempArr[1] + tempArr[2] + tempArr[3];
  }

  for(; i < SIZE ; ++i) {
    ret[i] = 0;
    for(; j < SIZE ; ++j)
      ret[i] += mat[i][j] * vec[j];
  }

	return ret;
}

int main(int argc, char **argv) {
	float mat[SIZE][SIZE];
	float vec[SIZE];
	int i, j;

	struct timeval start, end;

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++)
			mat[i][j] = (float)i / (float)(j + 1);
		vec[i] = (float)i;
	}

	gettimeofday(&start, NULL);

	for (i = 0; i < ITER; i++)
		mv_mult(mat, vec);

	gettimeofday(&end, NULL);

	i = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	printf("%d iterations, %d usec\n", ITER, i);

	return 0;
}

