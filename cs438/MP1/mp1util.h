/**
 * mp1util.h
 *
 * Utility header
 *
 * Author: Dennis J. McWherter, Jr.
 */

#ifndef MP1UTIL_H__
#define MP1UTIL_H__

#include "ConcurrentQueue.h"
#include <stdio.h>

#define FRAME_SIZE 1024
#define DATA_SIZE 1016
#define BITS_PER_BYTE 8

/**
 * Data to be sent to each thread
 */
typedef struct
{
  queue_t* queue;
  union {
    int sockfd;
    FILE* fp;
  } data;
} thread_data_t;

/**
 * Data to be sent through queue
 */
typedef struct
{
  void* data;
  short length;
  unsigned position;
} data_entry_t;

// Functions
int initConnection(const char* server, const char* port);
void initDataEntry(data_entry_t* entry, const char* data, short length, unsigned pos);
void destroyDataEntry(data_entry_t* entry);
short crc12(const char* frame);

#endif /* MP1UTIL_H__ */


