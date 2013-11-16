#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* change this program such that it takes an arbitrary long
  string input from stdin and saves it inside d */

static const int BLOCK_SIZE = 20;

int main()
{
	char s[BLOCK_SIZE], *d; //TODO lookup fgets
	int dsize = BLOCK_SIZE;
	d = calloc(dsize, sizeof(char));

	printf("Enter a string: ");
	
	while(1){ // for(;;){
		fgets(s, BLOCK_SIZE, stdin);
		strcat(d,s);
		if(strlen(s) < 19 || s[18] == '\n'){
			break;
		}else{
			dsize = dsize + BLOCK_SIZE - 1;  //can reuuse space for null byte
			d = realloc(d, dsize);
		}
	}
	printf("str = %s\n", d);
	free(d);
	return 0;
}
