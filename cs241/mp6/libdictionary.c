/* 
 * CS 241
 * The University of Illinois
 */

#define _GNU_SOURCE
#include <search.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "libdictionary.h"

static int compare(const void *a, const void *b)
{
	return strcmp(((const dictionary_entry_t *)a)->key, ((const dictionary_entry_t *)b)->key); 
}

static dictionary_entry_t *malloc_entry_t(const char *key, const char *value)
{
	dictionary_entry_t *entry = malloc(sizeof(dictionary_entry_t));
	entry->key = key;
	entry->value = value;

	return entry;
}

static dictionary_entry_t *dictionary_tfind(dictionary_t *d, const char *key)
{
	dictionary_entry_t tentry = {key, NULL};
	void *tresult = tfind((void *)&tentry, &d->root, compare);

	if (tresult == NULL)
		return NULL;
	else
		return *((dictionary_entry_t **)tresult);
}

static void dictionary_tdelete(dictionary_t *d, const char *key)
{
	dictionary_entry_t tentry = {key, NULL};
	tdelete((void *)&tentry, &d->root, compare);
}

static int dictionary_remove_options(dictionary_t *d, const char *key, int free_memory)
{
	dictionary_entry_t *entry = dictionary_tfind(d, key);

	if (entry == NULL)
		return NO_KEY_EXISTS;
	else
	{
		dictionary_tdelete(d, key);

		if (free_memory)
		{
			free((void *)entry->key);
			free((void *)entry->value);
		}
		free(entry);

		return 0;
	}
}

static void destroy_no_element_free(void *ptr)
{
	free(ptr);
}

static void destroy_with_element_free(void *ptr)
{
	dictionary_entry_t *entry = (dictionary_entry_t *)ptr;

	free((void *)entry->key);
	free((void *)entry->value);
	free(entry);
}


void dictionary_init(dictionary_t *d)
{
	d->root = NULL;
	pthread_mutex_init(&d->mutex, NULL);
}

int dictionary_add(dictionary_t *d, const char *key, const char *value)
{
	pthread_mutex_lock(&d->mutex);

	if (dictionary_tfind(d, key) == NULL)
	{
		tsearch((void *)malloc_entry_t(key, value), &d->root, compare);

		pthread_mutex_unlock(&d->mutex);
		return 0;
	}
	else
	{
		pthread_mutex_unlock(&d->mutex);
		return KEY_EXISTS;
	}
}

const char *dictionary_get(dictionary_t *d, const char *key)
{
	pthread_mutex_lock(&d->mutex);
	dictionary_entry_t *entry = dictionary_tfind(d, key);

	if (entry == NULL)
	{
		pthread_mutex_unlock(&d->mutex);
		return NULL;
	}
	else
	{
		pthread_mutex_unlock(&d->mutex);
		return entry->value;
	}
}

int dictionary_remove(dictionary_t *d, const char *key)
{
	pthread_mutex_lock(&d->mutex);
	int val = dictionary_remove_options(d, key, 0);
	pthread_mutex_unlock(&d->mutex);

	return val;
}

int dictionary_remove_free(dictionary_t *d, const char *key)
{
	pthread_mutex_lock(&d->mutex);
	int val = dictionary_remove_options(d, key, 1);
	pthread_mutex_unlock(&d->mutex);

	return val;
}

void dictionary_destroy(dictionary_t *d)
{
	tdestroy(d->root, destroy_no_element_free);
	d->root = NULL;

	pthread_mutex_destroy(&d->mutex);
}

void dictionary_destroy_free(dictionary_t *d)
{
	tdestroy(d->root, destroy_with_element_free);
	d->root = NULL;

	pthread_mutex_destroy(&d->mutex);
}

