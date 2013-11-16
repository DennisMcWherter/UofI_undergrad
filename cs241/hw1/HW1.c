/*		C No Evil Homework
Each question carries equal weight.

The assignment must be turned in and committed to your SVN by August 31, 2011
at 11:00am. No late submissions will be allowed.

Instructions on how to use SVN and other information relating to
online submission can be found on the course website.

http://www.cs.illinois.edu/class/cs241/handin.html
*/

/*
Part I
Each of these examples can appear inside C programs.

The ... means additional (irrelevant) instructions.

For each example, answer the following questions:
(a) What does the code attempt to do?

(b) Correct the code listed
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Assuming these preprocessors are intended at beginning for Problem14 */
#define multiply(x,y)	(x*y)
#define sum(x,y)	(x+y)

/*Goal: Set the memory location of p to x
 
*/
void Problem1(){
	int x = 0; /* Initialize x */
	int *p = &x;
}


/*Goal: Set the value of pointer "p" to float value 12.5f;

*/
void Problem2(){
	float *p = (float*)malloc(sizeof(float));
	*p = 12.5f;
	free(p);
}


/*Goal: Determine whether a float number falls in range (0, 1)

*/
void Problem3(){
	float a = 0.f;
/*	...*/
	if(0 < a && a < 1)
		printf("a is a value between 0 and 1.\n");
}

/*Goal: Determine whether values of x and y are equal

*/
void Problem4(){
	int x=5;
	int y=0;
/*	... */
	if(x == y)
		printf("x and y are equal.\n");
	else
		printf("x and y are different.\n");
	
}

/*Goal: Take an integer "x" and set int pointer "ip" to the memory location
 of the variable "x." Then cast the integer pointer and set the floating pointer
 "fp" to the memory location of "ip." Then set the value of "y" to the value of
 at memory location "fp." In short, convert int to float.

*/
void Problem5(){
	int x=1, *ip=0;
	float y=0.f, *fp=0;
/*	... */
    ip = &x;
    fp = (float*)ip; /* Memory location is same but we can only store either float or int */
    y = (float)*((int*)fp);
    
}

/*Goal: Loop from 0 to 10. Similar to for(int i=0;i<10;++i);

*/
void Problem6(){
	int a=0, b=10;
	while (a != b){
		/* ... */
		a++;
	}
	
}

/*Goal: Take an input with max length of 30 characters. Null byte at the end of 
 the char array to terminate the string.

*/
void Problem7(){
	char s[30];
	int len = scanf("%30s",s);
    memset(s, 0, sizeof(s));
	s[len] = '\0';
}


/*Goal: Set the value of integer pointer "x" to 0

*/
/*Problem 8 */
void reset(int *x){
	*x=0;
}

int main(){
/*	... */
	int x=1;
	reset(&x);
	printf("x is now 0.\n");
	return 0;
}


/*Goal: Allocate an array of 50 characters for a string

*/
void Problem9(){
	char *s = (char*)malloc(50*sizeof(char));
/*	... */
	s="this is a string";
	free(s);
}

/*Goal: Create a float array "values" of size 10 and store floating
  numbers 0, 1/10, ..., 9/10

*/
void Problem10(){
	float *values = (float*)malloc(10*sizeof(float));
    int i = 0,n = 10;
	for(i=0;i<n;i++)
		values[i] = ((float)i)/((float)n);
    free(values);
}

/*Goal: Create a 2-dimensional integer array (10x10) and store values of
  i*j in their corresponding locations (i.e. array[2][3] == array[3][2] == 6)

*/
void Problem11(){
	int **array;
	int i,j;
	array = (int**)malloc(10*sizeof(int *));
	for(i=0;i<10;i++) {
		array[i] =(int*) malloc(10*sizeof(int));
		for(j=0;j<10;j++)
			array[i][j] = i*j;
	}

	/* Free */
	for(i = 0; i < 10; ++i)
		free(array[i]);
	free(array);
}

/*Goal: Allocate a character string of length 10 then read in a max of 9 
  characters. Then set the integer value of "x" to 0, 1, or 2 based on whether
  or not the string equals "red" or "blue"

*/
void Problem12(){
	char *str = (char*)malloc(10*sizeof(char));
	int x = 0;
	scanf("%9s",str);
/*	... */
	if(strcmp(str, "blue") == 0)
		x = 1;
	else if(strcmp(str, "red") == 0)
		x = 2;
}

/*Goal: Write to an integer value "i" and determine which function call to execute
  based on its value (1 or 2). Otherwise, print "Unknown command."

*/
/* Stubs */
void function1(){}
void function2(){}
void Problem13(){
	int i = 0;
	/* ... */
	scanf("%d",&i);
	switch(i) {
		case 1: function1();
		case 2: function2();
		default: printf("Unknown command.\n");
	}
}


/*Goal: Use a macro to divide 100 by (x+1)*y and divide 100 by
  (x*y)+y

*/
void Problem14(){
	int x = 3, y = 5;
	int t = x+1;
	int p = x*y;
	int z = 100 / multiply(t, y);
	z = 100 / sum(p, y);
}

/*Goal: If "x" is a negative number, print "Invalud value" and stop execution

*/
void Problem15(){
	int x = 0;
	/* ... */
	if(x < 0){
		printf("invalid value.\n");
		exit(0);
	}
}

/*Goal: Attempt to append characters "abcd" to the character array "str"

*/
void Problem16(){
	char str[5];
    memset(str, 0, sizeof(str));
	strcat(str, "abcd");
}



/* Part II
Provide the code for the following functions.
You may use only the allowed operators specified in each problem.
You may not use any other operators, control constructs (if, do, while, ...),
or function calls.

You can only use constant values from 0L to 255L (0x0L to 0xFFL).

You may assume that your machine:
· Uses 2’s complement representation of integers
· Performs right shift arithmetically
· Uses either a 32- or a 64-bit representation of long integers
· Has unpredictable behavior when shifting a long integer by more
	than the word size

*/

long int clearBit(long int value, long int flag){
	/* returns value with all set bit positions from flag cleared
		using only the operators & ~ ^ | >> << ! + =
		
		For Example:  value=0xFF flag=0x55 -> retval=0xAA */
	return value & ~(flag);
}


long int isEqual(long int x, long int y){
	/* returns 1L if x==y, 0L otherwise
		using only the operators & ~ ^ | >> << ! + = */
	return !(x & ~y);
}

