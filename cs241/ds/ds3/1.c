#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* change this program such that it takes an arbitrary long
  string input from stdin and saves it inside d */

int main()
{
	char s[20], d[20];

	printf("Enter a string: ");
	//scanf("%s", s);
  fgets(s, 19, stdin);
  s[strlen(s)-1] = '\0';

	strcpy(d, s);
	printf("str = %s\n", d);

	return 0;
}
