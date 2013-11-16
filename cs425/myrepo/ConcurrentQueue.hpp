/**
 * ConcurrentQueue.h
 *
 * Simple concurrent queue wrapper around STL queue
 * Author: Dennis J. McWherter, Jr.
 */

#ifndef CONCURRENT_QUEUE_H__
#define CONCURRENT_QUEUE_H__

#include <queue>

#include "Poco/Mutex.h"

template<class T>
class ConcurrentQueue
{
public:
  /**
   * Virtual destructor
   */
  virtual ~ConcurrentQueue() {}

  /**
   * Enqueue
   *
   * @param data    Data to push onto the queue
   */
  void enqueue(T data)
  {
    mutex.lock();
    backend.push(data);
    mutex.unlock();
  }

  /**
   * Dequeue
   *
   * @return  The next element on the queue
   */
  T dequeue()
  {
    mutex.lock();
    T elem = backend.front();
    backend.pop();
    mutex.unlock();
    return elem;
  }

  /**
   * Clear the entire queue
   */
  void clear()
  {
    mutex.lock();
    while(!backend.empty())
      backend.pop();
    mutex.unlock();
  }
   
  /*
   * Check if queue is empty
   *
   * @return  True if empty, false otherwise
   */
  bool isEmpty()
  {
    mutex.lock();
    bool empty = backend.empty();
    mutex.unlock();
    return empty;
  }

private:
  Poco::Mutex mutex;
  std::queue<T> backend;
};

#endif /** CONCURRENT_QUEUE_H__ */
