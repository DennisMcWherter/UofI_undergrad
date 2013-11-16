/*
 * Machine Problem #1
 * CS 241
 */

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "libdictionary.h"

unsigned generateHash(const unsigned size, const char* key);
void resize(dictionary_t* d);
dictionary_entry_t * getEntry(dictionary_t* d, const char* key);
void strToLower(char* str);

/*
 * void dictionary_init(dictionary_t *d)
 *
 * Initialize your dictionary data structure d. If your data structure does
 * not require any initialization logic, this function may be empty.
 */
void dictionary_init(dictionary_t *d)
{
  unsigned i = 0;
  d->size    = 0;
  d->max     = 16;
  d->dictionary = (dictionary_entry_t**)malloc(d->max*sizeof(dictionary_entry_t*));
  for(i = 0; i < d->max; ++i)
    d->dictionary[i] = 0;
}

/*
 * int dictionary_add(dictionary_t *d, const char *key, const char *value)
 *
 * Adds the key-value pair (key, value) to the dictionary d, if and only if
 * the dictionary does not already contain a key with the same name as key.
 * This function should NOT make a copy of the Key and value. 
 *
 * Returns:
 *  # 0, on success.
 *  # KEY_EXISTS, if d already contains a key with the same name as key.
 */
int dictionary_add(dictionary_t *d, const char *key, const char *value)
{
  unsigned hash = 0;
  dictionary_entry_t * entry = getEntry(d, key);
  
  if(entry != 0)
    return KEY_EXISTS;
  
  if(((float)d->size/d->max) >= 0.7f) { /* Resize */
    resize(d);
  }

  hash  = generateHash(d->max, key);
  entry = d->dictionary[hash];

  if(entry != 0) {
    for(;entry->next!=0;entry=entry->next);
    entry->next = (dictionary_entry_t*)malloc(sizeof(dictionary_entry_t));
    entry->next->last = entry;
    entry = entry->next;
  } else {
    d->dictionary[hash] = (dictionary_entry_t*)malloc(sizeof(dictionary_entry_t));
    entry = d->dictionary[hash];
    entry->last = 0;
  }

  entry->next  = 0;
  entry->key   = (char*)key;
  entry->value = (char*)value;

  d->size++;

  return 0;
}

/*
 * int dictionary_parse(dictionary_t *d, char *key_value)
 *
 * Parses the key_value string and add the parsed key and value to the
 * dictionary. This function must make a call to dictionary_add() when
 * adding to the dictionary.
 *
 * Returns:
 *  # 0, on success (the key_value was parsed and added to the dictionary).
 *  # KEY_EXISTS, if the dictionary already contains a key with the same
 *                name as the KEY in key_value.
 *  # ILLEGAL_FORMAT, if the format of key_value is illegal.
 */
int dictionary_parse(dictionary_t *d, char *key_value)
{
  unsigned i  = 0;
  unsigned w  = 0;
  char* value = 0;

  if(strlen(key_value) <= 0)
    return ILLEGAL_FORMAT;
  
  for(i=0;key_value[i]!='\0';++i) {
    if(key_value[i] == ':') {
      if(w == 0)
        return ILLEGAL_FORMAT; /* Zero-lengthed key */
      if(key_value[i+1] != ' ')
        return ILLEGAL_FORMAT;
      break;
    } else if(key_value[i+1] == '\0') {
      return ILLEGAL_FORMAT;
    }
    if(key_value[i] != ' ' && key_value[i] != ':') {
      w = 1;
    }
  }

  value = key_value + (i+2); /* Extract this substring */
  key_value[i] = '\0'; /* We will null terminate key_value early so it becomes key */
 
  return dictionary_add(d, key_value, value); /* Returns 0 or KEY_EXISTS */
}

/*
 * const char *dictionary_get(dictionary_t *d, const char *key)
 *
 * Returns the value of the key-value element with the key name key from
 * the dictionary d. If the key does not exist, this function should
 * return NULL. 
 *
 * Returns
 *  # the value of the key-value element, if the key name key exists
 *                                        in the dictionary
 *  # NULL, otherwise.
 */
const char *dictionary_get(dictionary_t *d, const char *key)
{
  dictionary_entry_t * entry = getEntry(d, key);
  if(entry != 0)
    return entry->value;
  return NULL;
}

/*
 * int dictionary_remove(dictionary_t *d, const char *key)
 *
 * Removes the element with the key key from the dictionary d,
 * if d contains the key. 
 *
 * Returns
 *  # 0, on success.
 *  # NO_KEY_EXISTS, if the dictionary did not contain key.
 */
int dictionary_remove(dictionary_t *d, const char *key)
{
  unsigned hash = 0;
  dictionary_entry_t * entry = getEntry(d, key);

  if(entry == 0)
    return NO_KEY_EXISTS;
  
  if(entry->last == 0) {
    hash = generateHash(d->max, key);
    d->dictionary[hash] = entry->next;
    if(entry->next != 0)
      entry->next->last = 0;
  } else {
    entry->last->next = entry->next;
    if(entry->next != 0)
      entry->next->last = entry->last;
  }
  
  free(entry);
  
  d->size--;
  
  return 0;
}

/*
 * void dictionary_destroy(dictionary_t *d)
 *
 * Frees any memory associated with the dictionary d. 
 */
void dictionary_destroy(dictionary_t *d)
{
  unsigned i = 0;
  dictionary_entry_t * entry = 0;
  dictionary_entry_t * next  = 0;
  
  for(i = 0; i < d->max; ++i) {
    entry = d->dictionary[i];
    if(entry == 0)
      continue;
    for(;entry!=0;entry=next) {
      next = entry->next;
      free(entry);
    }
  }
  
  free(d->dictionary);
}

dictionary_entry_t * getEntry(dictionary_t* d, const char* key)
{
  unsigned hash = 0;
  unsigned same = 0;
  unsigned i    = 0;
  const unsigned size = strlen(key);
  dictionary_entry_t * entry = 0;

  if(key == 0 || *key == '\0')
      return 0;
  
  hash  = generateHash(d->max, key);
  entry = d->dictionary[hash];
  
  for(;entry!=0;entry=entry->next) {
    if(strlen(entry->key) != size)
      continue;
    same = 1;
    for(i = 0 ; i < size ; ++i) {
      if(tolower(key[i]) != tolower(entry->key[i])) {
          same = 0;
          break;
      }
    }
    if(same == 1)
      return entry;
  } 
  return 0;
}

unsigned generateHash(const unsigned size, const char* key)
{
  unsigned i    = 0;
  unsigned hash = 0;
  for(i = 0; i < strlen(key)-1; ++i) {
    hash += tolower(key[i]) % 33;
  }
  hash %= size;
  return hash;
}

void resize(dictionary_t* d)
{
  int hash = 0;
  unsigned i = 0;
  const unsigned oldMax = d->max;
  const unsigned newMax = 2*oldMax;
  dictionary_entry_t ** dict = (dictionary_entry_t**)malloc(newMax*sizeof(dictionary_entry_t*));
  dictionary_entry_t ** old  = d->dictionary;
  dictionary_entry_t * entry = 0;
  dictionary_entry_t * ent2  = 0;
  dictionary_entry_t * next  = 0;
  
  for(i = 0; i < newMax; ++i)
    dict[i] = 0;
  
  for(i = 0; i < oldMax; ++i) {
    entry = old[i];
    
    if(entry == 0)
      continue;
    
    for(;entry!=0;entry=next) {
      hash = generateHash(newMax, entry->key);
      next = entry->next;
      if(dict[hash] == 0) {
        dict[hash]  = entry;
        entry->last = 0;
      } else {
        ent2 = dict[hash];
        for(;ent2->next!=0;ent2=ent2->next);
        ent2->next = entry;
        entry->last = ent2;
      }
      entry->next = 0;
    }
  }
  
  free(old);
  d->max = newMax;
  d->dictionary = dict;
}

