#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

typedef float v4sf __attribute__ ((vector_size (4*sizeof(float))));

#define	SIZE	32
#define	ITER	100000

#define	M_ITER	200
#define	M_MAG	4.0

// mandelbrot() takes a set of SIZE (x,y) coordinates - these are actually
// complex numbers (x + yi), but we can also view them as points on a plane.
// It then runs 200 iterations of f, using the (x,y) point, and checks
// the magnitude of the result.  If the magnitude is over 4.0, it assumes
// that the function will diverge to infinity.
int *mandelbrot(float x[SIZE], float y[SIZE]) {
	static int ret[SIZE];
	float x1, y1, x2, y2, xArr[4], yArr[4];
	int i, j;
  v4sf accX, accY, temp1, temp2;

  __builtin_ia32_xorps(accX, accX);
  __builtin_ia32_xorps(accY, accY);

	for (i = 0; i < SIZE-3; i+= 4) {
		x1 = y1 = 0.0;

    temp1 = __builtin_ia32_loadups(&x[i]);
    temp2 = __builtin_ia32_loadups(&y[i]);

		// Run M_ITER iterations
		for (j = 0; j < M_ITER; j++) {
			// Calculate the real part of (x1 + y1 * i)^2 + (x + y * i)
			//x2 = (x1 * x1) - (y1 * y1) + x[i];
      accX = __builtin_ia32_subps(__builtin_ia32_mulps(temp1, temp1), __builtin_ia32_mulps(temp2, temp2));

			// Calculate the imaginary part of (x1 + y1 * i)^2 + (x + y * i)
			//y2 = 2 * (x1 * y1) + y[i];
      accY = __builtin_ia32_addps(__builtin_ia32_addps(__builtin_ia32_mulps(temp1, temp2), __builtin_ia32_mulps(temp1, temp2)), temp2);

      __builtin_ia32_storeups(xArr, accX);
      __builtin_ia32_storeups(yArr, accY);

      x2 = xArr[0] + xArr[1] + xArr[2] + xArr[3];
      y2 = yArr[0] + yArr[1] + yArr[2] + yArr[3];

			// Use the new complex number as input for the next iteration
			x1 = x2;
			y1 = y2;
		}

		// caculate the magnitude of the result
		// We could take the square root, but instead we just
		// compare squares
		ret[i] = ((x2 * x2) + (y2 * y2)) < (M_MAG * M_MAG);
	}

  for(; i < SIZE; ++i) {
    for(j = 0 ; j < M_ITER; ++j) {
      x2 = (x1 * x1) - (y1 * y1) + x[i];
      y2 = 2 * (x1 * y1) + y[i];
      x1 = x2;
      y1 = y2;
    }
    ret[i] = ((x2 * x2) + (y2 * y2)) < (M_MAG * M_MAG);
  }

	return ret;
}

int main(int argc, char **argv) {
	float x[SIZE];
	float y[SIZE];
	int i;

	struct timeval start, end;

	for (i = 0; i < SIZE; i++) {
		x[i] = 1.0 / (float)(i + 3);
		y[i] = 1.0 / (float)(i + 3);
	}

	gettimeofday(&start, NULL);

	for (i = 0; i < ITER; i++)
		mandelbrot(x, y);

	gettimeofday(&end, NULL);

	i = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	printf("%d iterations, %d usec\n", ITER, i);

	return 0;
}

