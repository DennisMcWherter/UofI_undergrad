/*
 * Returning a struct from a (void *)
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>

typedef struct _letter_count_t
{
	int vowels;
	int consonants;
} letter_count_t;

/*
 * Counts the number of vowels and consonants in the provided input string.
 * Returns a newly malloc()'d letter_count_t structure.
 */
void *worker(void *ptr)
{
	char *s = (char *)ptr;
	int i;
	letter_count_t *letter_count = malloc(sizeof(letter_count_t));
	letter_count->vowels = 0;
	letter_count->consonants = 0;

	for (i = 0; i < strlen(s); i++)
	{
		char c = s[i];

		if (isalpha(c))
		{
			c = tolower(c);

			if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
				letter_count->vowels++;
			else
				letter_count->consonants++;
		}
	}

	return letter_count;
}

void main()
{
	pthread_t t;
	char *str = "The quick brown fox jumped over the lazy dog.";
	pthread_create(&t, NULL, worker, str);

	letter_count_t *letter_count;
	//pthread_join(t, ???);

	printf("Vowels: %d\n", letter_count->vowels);
	printf("Consonants: %d\n", letter_count->consonants);
}

