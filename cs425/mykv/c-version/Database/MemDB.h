/**
 * MemDB.h
 *
 * In-memory database
 */

#ifndef NETWORKING_MEMDB_H__
#define NETWORKING_MEMDB_H__

#include <pthread.h>

/**
 * Database entry
 *
 * @entry key   The key associated with entry
 * @entry value The data associated with entry
 * @entry size  Size of the data in bytes
 */
typedef struct _memdb_entry
{
  unsigned key;
  void* value;
  unsigned size;
  struct _memdb_entry* next;
  struct _memdb_entry* prev;
} memdb_entry_t;

/**
 * Simple in-memory database
 *   - linked-list implementation
 *
 *   Implementation details:
 *   O(1) insert
 *   O(n) remove
 *   O(n) lookup
 */
typedef struct _memdb_t
{
  memdb_entry_t* head;
  unsigned size;
} memdb_t;

/**
 * NOTE: Global variables are not necessarily
 *   the best solution by any coding standard.
 *   However, for a very specific-use server
 *   such as this where _exactly_ one database
 *   is used at all times, it seems most practical
 *   to implement it like this.
 */

/**
 * The methods below are thread-safe.
 * They operate on a single database
 * for the whole server
 */

/**
 * Initialize the database
 * NOTE: Method should only be called _ONCE_
 *  before threads are spawned.
 *
 *  @return void
 */
void memdb_init();

/**
 * Destroy the database
 * NOTE: Should only be called once at cleanup
 *
 * @return void
 */
void memdb_destroy();

/**
 * Insert entry into the database. If entry
 *  already exists at that key, overwrite it.
 *
 * @param key   Key to store value
 * @param data  The data to store
 * @param size  Size of the data
 * @return 1 on success. 0 otherwise.
 */
int memdb_insert(unsigned key, void* data, unsigned size);

/**
 * Remove a key
 *
 * @param key   Key to remove
 * @return void
 */
void memdb_remove(unsigned key);

/**
 * Lookup entry
 *
 * @param key Key to lookup
 * @return  An entry if one exists, NULL otherwise.
 */
memdb_entry_t* memdb_lookup(unsigned key);

/**
 * Get number of entries in database
 *
 * @return  The number of entries in db
 */
unsigned memdb_size();

#endif /** NETWORKING_MEMDB_H__ */

