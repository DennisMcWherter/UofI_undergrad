#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

int main(){
	// initializing the vector 'history'
	vector_t history;
	vector_init(&history);

	int i,j;

	// getting 10 strings and storing them inside the vector 'history'
	for(i = 0; i < 10; i++){

		printf("Enter element: ");
		// create space for the element
		char* str = (char*)malloc(sizeof(char)*100);
		scanf("%s", str);

		// appending the element to the history
		vector_append(&history, str);

		printf("After append, history size = %d\n", vector_size(&history));

		// printing the entire history after the append
		printf("History after add:\n");
		for(j = 0; j < vector_size(&history); j++){
			char* teststr = vector_at(&history, j);
			printf("Element at %d: %s\n", j, teststr);
		}
	}

	// free the elements
	for(j = 0; j < vector_size(&history); j++){
		char* teststr = vector_at(&history, j);
		free(teststr);
	}

	// free the vector
	vector_destroy(&history);
	return 0;
}
