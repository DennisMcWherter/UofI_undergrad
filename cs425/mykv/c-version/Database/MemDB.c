/**
 * MemDB.c
 *
 * Implementation of synchronized in-memory database
 */

#include <stdlib.h>
#include <string.h>

#include "MemDB.h"

static memdb_t kvDatabase;
static pthread_mutex_t kvDB_mutex;

void memdb_init()
{
  pthread_mutex_init(&kvDB_mutex, NULL);
  kvDatabase.head = NULL;
  kvDatabase.size = 0;
}

void memdb_destroy()
{
  memdb_entry_t* curr = NULL;
  memdb_entry_t* tmp  = NULL;
  pthread_mutex_lock(&kvDB_mutex);

  curr = kvDatabase.head;

  while(curr != NULL) {
    tmp  = curr;
    curr = curr->next;
    free(tmp->value);
    free(tmp);
  }

  pthread_mutex_unlock(&kvDB_mutex);
  pthread_mutex_destroy(&kvDB_mutex);
}

int memdb_insert(unsigned key, void* data, unsigned size)
{
  memdb_entry_t* entry = malloc(sizeof(memdb_entry_t));
  void* value = malloc(size);

  /** Free old data */
  memdb_remove(key);

  /** Critical section */
  pthread_mutex_lock(&kvDB_mutex);
  
  /* Copy data to persistent memory */
  memcpy(value, data, size);

  /* Make entry */
  entry->key   = key;
  entry->value = value;
  entry->size  = size;
  entry->next  = NULL;
  entry->prev  = NULL;
  
  /* Always insert at the head */
  entry->next = kvDatabase.head;
  kvDatabase.head = entry;
  kvDatabase.size++;

  if(entry->next != NULL)
    entry->next->prev = entry;
  
  pthread_mutex_unlock(&kvDB_mutex);
  
  return 1;
}

void memdb_remove(unsigned key)
{
  memdb_entry_t* entry = NULL;
  
  pthread_mutex_lock(&kvDB_mutex);
  
  entry = kvDatabase.head;

  while(entry != NULL) {
    if(entry->key == key)
      break;
    entry = entry->next;
  }

  if(entry != NULL) {
    if(entry == kvDatabase.head) {
      kvDatabase.head = entry->next;
    } else {
      entry->prev->next = entry->next;
      if(entry->next != NULL)
        entry->next->prev = entry->prev;
    }
    free(entry->value);
    free(entry);
    kvDatabase.size--;
  }
  
  pthread_mutex_unlock(&kvDB_mutex);
}

memdb_entry_t* memdb_lookup(unsigned key)
{
  memdb_entry_t* curr = NULL;

  pthread_mutex_lock(&kvDB_mutex);
  
  curr = kvDatabase.head;
  
  while(curr != NULL) {
    if(curr->key == key)
      break;
    curr = curr->next;
  }
  
  pthread_mutex_unlock(&kvDB_mutex);

  return curr;
}

unsigned memdb_size()
{
  unsigned size = 0;

  pthread_mutex_lock(&kvDB_mutex);

  size = kvDatabase.size;

  pthread_mutex_unlock(&kvDB_mutex);

  return size;
}

