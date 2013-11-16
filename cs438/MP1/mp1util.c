/**
 * mp1util.c
 *
 * MP1 Utility implementation
 *
 * Author: Dennis J. McWherter, Jr.
 */

#include "mp1util.h"

#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * Initialize a connection to a server
 *
 * @param server    Server to connect
 * @param port      Port to connect
 *
 * @return valid file descriptor or -1 on error
 */
int initConnection(const char* server, const char* port)
{
  int sockfd = -1;
  struct addrinfo* res, hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if(getaddrinfo(server, port, &hints, &res) == -1) {
    perror("getaddrinfo:");
    return -1;
  }

  if(res == NULL) {
    perror("res is null (port out of range?)");
    return -1;
  }

  if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
    perror("socket:");
    return -1;
  }

  if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    close(sockfd);
    freeaddrinfo(res);
    perror("Connect:");
    return -1;
  }

  freeaddrinfo(res);

  return sockfd;
}

/**
 * Initialize a data entry with given parameters
 *
 * @param entry     Entry to initialize
 * @param data      Data to add
 * @param length    Length of data
 * @param position  Position of data
 */
void initDataEntry(data_entry_t* entry, const char* data, short length, unsigned position)
{
  if(entry == NULL)
    return;

  entry->data = NULL;
  entry->length = length;
  entry->position = position;
  if(data != NULL) {
    entry->data = malloc(DATA_SIZE);
    memcpy(entry->data, data, DATA_SIZE);
  }
}

/**
 * Destroy a given data entry
 *
 * @param entry   Entry to destroy
 */
void destroyDataEntry(data_entry_t* entry)
{
  if(entry == NULL)
    return;
  free(entry->data);
}

/**
 * CRC-12 Check
 *
 * NOTE: The method assumes that data is in
 *    network byte order (big-endian).
 *
 * @param frame    The frame to check
 *
 * @return 0 for no errors, the error otherwise.
 */
short crc12(const char* frame)
{
  if(frame == NULL)
    return 0;

  short rem = 0;

  static const short poly = 0x180d;
  static const short mask = 0x800;

  short code = ntohs((*((short*)&frame[FRAME_SIZE-2]))) & 0xfff;

  int i = 0;
  int j = 0;

  // Calculate the CRC then XOR the code at the end
  // Adopted pseudocode from: http://en.wikipedia.org/wiki/Computation_of_CRC
  for(i = 0 ; i < FRAME_SIZE - 2 ; ++i) {
    rem ^= (*frame++ << 4);
    for(j = 0 ; j < BITS_PER_BYTE ; ++j) {
      rem = (rem & mask) ? (rem << 1) ^ poly : (rem << 1);
      rem &= 0xfff;
    }
  }

  // Make sure the remainders match - if not, something is wrong
  rem ^= code;

  return rem;
}

