/*
 * Creating and using function pointers
 */

#include <stdio.h>

double add(double x, double y)
{
	return x + y;
}

double subtract(double x, double y)
{
	return x - y;
}

double multiply(double x, double y)
{
	return x * y;
}

double divide(double x, double y)
{
	if (y == 0) return 0;
	else return x / y;
}

// We can add any other functions that fit the pattern:
//     double _____(double ___, double ___)
//     {
//        ...
//     }

double do_math(double x, double y, double (*function)(double, double))
{
	return function(x, y);
}


void main()
{
}
