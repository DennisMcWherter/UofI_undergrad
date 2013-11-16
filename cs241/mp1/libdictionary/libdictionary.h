/*
 * Machine Problem #1
 * CS 241
 */

#ifndef LIBDICTIONARY_H__
#define LIBDICTIONARY_H__

#define NO_KEY_EXISTS 1
#define KEY_EXISTS 2
#define ILLEGAL_FORMAT 3

typedef struct _dictionary_entry_t
{
    struct _dictionary_entry_t * next;
    struct _dictionary_entry_t * last;
    char* key;
    char* value;
} dictionary_entry_t;

typedef struct _dictionary_t
{
	/* You may find this dictionary_entry_t pointer useful.
           You are not required to use it and may remove it
	   but your data structure MUST shrink/expand based on
	   the size of entries. */
	dictionary_entry_t **dictionary;
  unsigned size;
  unsigned max;
} dictionary_t;

void dictionary_init(dictionary_t *d);
int dictionary_add(dictionary_t *d, const char *key, const char *value);
int dictionary_parse(dictionary_t *d, char *key_value);
const char *dictionary_get(dictionary_t *d, const char *key);
int dictionary_remove(dictionary_t *d, const char *key);
void dictionary_destroy(dictionary_t *d);

#endif
