/**
 * MemDB.h
 *
 * In-memory concurrent database
 * Author: Dennis J. McWherter, Jr.
 */

#ifndef MEMDB_H__
#define MEMDB_H__

#include <map>
#include <sstream>
#include <string>

#include "Poco/Mutex.h"

// Use Poco's SHA1 implementation
// to compute hash on keys
#include "Poco/DigestEngine.h"
#include "Poco/SHA1Engine.h"

/**
 * Concurrent database structure
 */
template<class T>
class MemDB
{
public:
  /**
   * Constructor
   *
   * NOTE: This constructor only works if R data type
   *    has a default constructor
   */
  MemDB()
  {
  }

  /**
   * Constructor
   * Copies the sentinel with given values
   *
   * NOTE: This constructor will invoke the copy constructor
   *    of the R data type.
   *
   * @param sentinel    Some sentinel to pass in
   */
  MemDB(const T& sentinel)
    : sentinel(sentinel)
  {
  }

  /**
   * Destructor
   */
  virtual ~MemDB()
  {
  }

  /**
   * Assignment operator
   *
   * @param rhs   The object to be copied
   */
  virtual MemDB& operator=(const MemDB& rhs)
  {
    if(&rhs != this) {
      // Just keep our own mutex
      db = rhs.db;
      sentinel = rhs.sentinel;
    }
    return *this;
  }

  /**
   * Insert into the database
   *
   * @param key     Key value to insert data at
   * @param data    Data to insert
   */
  void insert(const std::string& key, const T& data)
  {
    mutex.lock();
    db[key] = data;
    mutex.unlock();
  }

  /**
   * Remove entry from database
   *
   * @param key     Key value to remove
   */
  void remove(const std::string& key)
  {
    mutex.lock();
    typename std::map<std::string,T>::iterator it = db.find(key);
    if(it != db.end())
      db.erase(it);
    mutex.unlock();
  }

  /**
   * Lookup an entry from database
   *
   * @param key     Key to remove from database
   * @return  The data at associated key. If nothing found
   *      returns sentinel.
   */
  const T& lookup(const std::string& key)
  {
    mutex.lock();
    typename std::map<std::string,T>::iterator it = db.find(key);
    bool notFound  = (it == db.end());
    const T& value = (notFound) ? sentinel : it->second;
    mutex.unlock();
    return value;
  }

  /**
   * Check if returned value is sentinel (i.e. recevied
   *  unusable value).
   *
   * @param value   The value to check
   * @return  True if the value is not the sentinel value.
   *      otherwise false.
   */
  bool isGoodValue(const T& value) const
  {
    return (&value != &sentinel);
  }

  /**
   * Lock the database
   *
   * This should be used to lock the database
   * for large operations which need to be atmoic
   * (i.e. full database replication).
   * Release lock with the unlock() method
   */
  void lock()
  {
    mutex.lock();
  }

  /**
   * Unlock the database
   *
   * To be used if the lock() method was used
   */
  void unlock()
  {
    mutex.unlock();
  }

  /** These methods break encapsulation, but it's faster than programming a custom iterator */
  /**
   * Get the beginning of the DB
   *
   * NOTE: This method is NOT thread-safe,
   * class lock() before calling this method
   */
  typename std::map<std::string, T>::const_iterator begin() const
  {
    return db.begin();
  }

  /**
   * Get the end of the DB
   *
   * NOTE: This method is NOT thread-safe,
   * class lock() before calling this method
   */
  typename std::map<std::string, T>::const_iterator end() const
  {
    return db.end();
  }

  /**
   * Non-const versions of the corresponding const functions
   */
  typename std::map<std::string, T>::iterator rawBegin()
  {
    return db.begin();
  }

  typename std::map<std::string, T>::iterator rawEnd()
  {
    return db.end();
  }

  /**
   * Get the size of the database
   *
   * NOTE: This method is NOT thread-safe,
   *  lock the database before using it.
   *
   * @return  The size of the database
   */
  unsigned size()
  {
    return db.size();
  }

  /**
   * Get a non-const reference to the element at the
   * position. Returns sentinel if none exists.
   *
   * NOTE: This method is _NOT_ synchronized. You must
   *  first lock the database.
   *
   * @param key   The key to lookup
   */
  typename std::map<std::string, T>::iterator rawLookup(const std::string& key)
  {
    return db.find(key);
  }

  /**
   * Unlocked insert
   *
   * @param key   Key to insert
   * @param value Value to insert
   */
  void rawInsert(const std::string& key, const T& val)
  {
    db[key] = val;
  }

  /**
   * Get numeric hash value
   *
   * @param key       Key to hash
   * @param maxKeys   Maximum number of keys (limits hash value)
   *        Default = -1 which is disabled
   * @return  Numeric hash value
   */
  static unsigned getHash(const std::string& key, int maxKeys=-1)
  {
    Poco::SHA1Engine hash;
    std::stringstream ss;
    unsigned hashNumeric = 0;

    hash.update(key);

    std::string hashVal = Poco::SHA1Engine::digestToHex(hash.digest());

    ss << hashVal;
    ss >> hashNumeric;

    if(maxKeys != -1)
      hashNumeric %= maxKeys;

    return hashNumeric;
  }

private:
  MemDB(const MemDB&){} // Not implemented

  std::map<std::string,T> db; // Structure to hold the data
  Poco::Mutex mutex;
  T sentinel;
};

#endif /** MEMDB_H__ */
