/*
 * Machine Problem #1
 * CS 241
 */

/*
 * You SHOULD modify this file and add more test cases!  The autograder
 * will run many more test cases than the 8 provided below.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "libdictionary/libdictionary.h"

int main()
{
	/*
	 * Initialize the dictionary data structure.
	 */
	dictionary_t dictionary;
	dictionary_init(&dictionary);


	/*
	 * Preform some basic actions
	 */
	int result;
	const char *s;

	/* _add() */
	result = dictionary_add(&dictionary, "key", "value");
	if (result != 0) { printf("_add() failed, and it should have been successful.\n"); }
	else { printf("_add(): OKAY!\n"); }

	result = dictionary_add(&dictionary, "key2", "value");
	if (result != 0) { printf("_add() failed, and it should have been successful.\n"); }
	else { printf("_add(): OKAY!\n"); }

	result = dictionary_add(&dictionary, "key3", "value");
	if (result != 0) { printf("_add() failed, and it should have been successful.\n"); }
	else { printf("_add(): OKAY!\n"); }

	result = dictionary_add(&dictionary, "key", "value2");
	if (result == 0) { printf("_add() was successful, and it should've failed.\n"); }
	else { printf("_add(): OKAY!\n"); }

	/* _remove() */
	dictionary_remove(&dictionary, "key3");

	/* _get() */
	s = dictionary_get(&dictionary, "non-existant");
	if (s != NULL) { printf("_get() was successful, and it should've failed.\n"); }
	else { printf("_get(): OKAY!\n"); }

	s = dictionary_get(&dictionary, "key");
	if (s == NULL || strcmp(s, "value") != 0) { printf("_get() failed or was not the expected result.\n"); }
	else { printf("_get(): OKAY!\n"); }

	/* _parse() */
	char *s1 = malloc(100);
	strcpy(s1, "key3: value");
	result = dictionary_parse(&dictionary, s1);
	if (result != 0) { printf("_parse() failed, and it should have been successful.\n"); }
	else { printf("_parse(): OKAY!\n"); }

	char *s2 = malloc(100);
	strcpy(s2, "bad key-value");
	result = dictionary_parse(&dictionary, s2);
	if (result == 0) { printf("_parse() was successful, and it should've failed.\n"); }
	else { printf("_parse(): OKAY!\n"); }

	/* _get() */
	s = dictionary_get(&dictionary, "key3");
	if (s == NULL || strcmp(s, "value") != 0) { printf("_get() failed or was not the expected result.\n"); }
	else { printf("_get(): OKAY!\n"); }

    result = dictionary_parse(&dictionary, "hola, :senor");
    if(result == 0) printf("Bad!\r\n");
    else printf("Success\r\n");

    strcpy(s1, "w00t: d00ddd");
    result = dictionary_parse(&dictionary, s1);
    //char * z = "w00t: d00ddd";
    s = dictionary_get(&dictionary, "w00t");
    if(s == 0 || strcmp(s, "d00ddd") != 0) printf("Bad: %s\r\n", s);
    else printf("Success\r\n");

    char * s3 = (char*)malloc(100);
    strcpy(s3, "    : sdasda");
    result = dictionary_parse(&dictionary, s3);
    s = dictionary_get(&dictionary, "");
    if(s != 0) printf("Bad: %s\r\n", s);
    else printf("Success\r\n");

    char* s4 = (char*)malloc(100);
    strcpy(s1, "ad: (&DAS^\\x");
    if(dictionary_parse(&dictionary, s1) != 0)
        printf("Error\r\n");
    s = dictionary_get(&dictionary, "ad");
    strcpy(s1, "w00t1: test");
    dictionary_parse(&dictionary, s1);
 strcpy(s4, "w00t1");
    s = dictionary_get(&dictionary, s4);
    if(s == 0 || strcmp(s, "test") != 0)
        printf("Error3\r\n");
    else
        printf("Success\r\n");

    strcpy(s1, "w00t1: test2324334");
    if((result = dictionary_parse(&dictionary, s1)) != KEY_EXISTS)
        printf("Error: %d\r\n", result);
    else printf("Success\r\n");


    /**** BEGIN "Robust" Test Cases ****/
    // Test 1
    printf("\r\n\r\n=== BEGIN ROBUST ===\r\n\r\n");
    char x[] = "Test Me: 57 ad9 X";
    result = dictionary_parse(&dictionary, x);
    if(result != 0) printf("Error: Test 1, %d\r\n", result);
    else printf("Test 1 Passed!\r\n");

    // Test 2
    s = dictionary_get(&dictionary, "Test Me");
    if(s != 0 && strcmp(s, "57 ad9 X") == 0) printf("Test 2 Passed!\r\n");
    else printf("Error: Test2\r\n");

    // Test 3
    result = dictionary_add(&dictionary, "Test me", "test_value");
    s = dictionary_get(&dictionary, "tESt mE");
    if(result != KEY_EXISTS || strcmp(s, "57 ad9 X") != 0) printf("Error: Test 3, %d, %s\r\n", result, s);
    else printf("Test 3 Passed!\r\n");

    // Test 4
    char y[] = "TeSt Me: d00d another one <3";
    result = dictionary_parse(&dictionary, y);
    if(result != KEY_EXISTS) printf("Error: Test 4\r\n");
    else printf("Test 4 Passed!\r\n");

    // Test 5
    dictionary_remove(&dictionary, "TeSt ME");
    s = dictionary_get(&dictionary, "test me");
    if(s != 0) printf("Error: Test 5\r\n");
    else printf("Test 5 Passed!\r\n");


	/*
	 * Free up the memory used by the dictionary and close the file.
	 */
	dictionary_destroy(&dictionary);
	free(s1);
	free(s2);
    free(s3);
    free(s4);

	return 0;
}

