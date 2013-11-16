/**
 * networking.c
 *
 * Implementation for networking-related
 * methods
 *
 * Author: Dennis J. McWherter, Jr. (dmcwhe2)
 */

#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "networking.h"

/**
 * Initialize the UDP server
 *
 * @param port  Port to bind the UDP server to
 *
 * @return  The socket file descriptor on success,
 *        a value < 0 otherwise.
 */
int initUDPServer(const char* port)
{
  int sockfd = -1;
  int success = 0;
  int optval = 1;
  struct addrinfo* result, *s;

  if(getaddrinfo(NULL, port, NULL, &result) != 0) {
    perror("getaddrinfo");
    return -1;
  }

  if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    freeaddrinfo(result);
    perror("socket creation");
    return -1;
  }

  // For sake of reuse
  if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
    perror("setsockopt");
    freeaddrinfo(result);
    close(sockfd);
    return -1;
  }

  // Just hope we can bind to something in our list
  for(s = result ; s != NULL && success != 1 ; s = s->ai_next) {
    if(bind(sockfd, s->ai_addr, s->ai_addrlen) != -1)
      success = 1;
  }

  freeaddrinfo(result);

  // Could not bind
  if(success == 0) {
    close(sockfd);
    return -1;
  }

  return sockfd;
}

/**
 * Initialize a TCP connection to another host
 *
 * @param host    Host to connect to
 * @param port    Port to connect on
 *
 * @return The file descriptor with an open connection
 *    to the host or a value < 0 on failure.
 */
int initTCPConnect(const char* host, const char* port)
{
  int sockfd = -1;
  struct addrinfo hints;
  struct addrinfo* result;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;

  if(getaddrinfo(host, port, &hints, &result) != 0) {
    perror("getaddrinfo");
    return -1;
  }

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket creation");
    freeaddrinfo(result);
    return -1;
  }

  if(connect(sockfd, result->ai_addr, result->ai_addrlen) == -1) {
    perror("connect");
    close(sockfd);
    sockfd = -1;
  }

  freeaddrinfo(result);

  return sockfd;
}

/**
 * Send a data buffer over a TCP socket
 * this method guarantees all byres are sent
 * if possible
 *
 * @param sockfd    Open TCP connection
 * @param buf       Buffer to send
 * @param len       Length of buffer
 *
 * @return  The total number of bytes sent on success.
 *  0 if the socket closed, -1 on error.
 */
int sendTCP(int sockfd, const void* buf, size_t len)
{
  int sent = 0;
  int total = 0;

  while(total < len) {
    sent = send(sockfd, buf + total, len - total, 0);

    if(sent == 0 || sent == -1)
      return sent;

    total += sent;
  }

  return len;
}

/**
 * Convenience method for receiving single-delimited
 * data. May not always be best method to receive
 * data from server (unless you can ensure nothing
 * will be sent until after all of this is read with
 * delimiter)
 *
 * @param sockfd    Open TCP socket
 * @param delim     Delimiter character/string to match
 * @param max       The maximum number of bytes to recv before if no delim found
 *                    Any value < 1 disables max
 * @param timeout   The timeout value of the method (in seconds). -1 disables timeout
 *
 * @return  A buffer with the received data.
 *
 * NOTE: The user application is responsible for freeing this memory
 */
char* recvTCP(int sockfd, const char* delim, int max, int timeout)
{
  int rcvd = 0;
  int total = 0;
  int done = 0;
  unsigned datalen = 8; // Arbitrary starting point
  char* data = malloc(datalen * sizeof(char));
  const unsigned dlen = strlen(delim);
  struct timeval tv;
  fd_set rdfds;

  while(((max > 0) ? total < max : 1) && done == 0) {

    if(datalen <= total) {
      datalen *= 2;
      data = realloc(data, datalen);
    }

    if(timeout > -1) {
      tv.tv_sec  = timeout;
      tv.tv_usec = 0;
      FD_ZERO(&rdfds);
      FD_SET(sockfd, &rdfds);
      select(sockfd + 1, &rdfds, NULL, NULL, &tv);
      if(!FD_ISSET(sockfd, &rdfds)) {
        free(data);
        return NULL;
      }
    }

    // Read 1 byte at a time as an attempt to read arbitrarily
    rcvd = recv(sockfd, data + total, 1, 0);

    if(rcvd == 0 || rcvd == -1) {
      free(data);
      return NULL;
    }

    // Use this to limit our recv in case of an error
    total += rcvd;

    // Check if most recent character read gives
    // us the delimiter
    if(total >= dlen) {
      // Set done and strip delim/terminate
      if(strncmp(data + total - dlen, delim, dlen) == 0) {
        done = 1;
        *(data + total - dlen) = '\0';
      }
    }
  }

  return data;
}

/**
 * Receive from UDP connection
 *
 * @param sockfd    Open UDP socket
 * @param buf       Buffer to fill with new information
 * @param len       Length of buffer
 * @return  The number of bytes received or -1 on error.
 * NOTE: This method uses a timeout through select()
 */
int recvUDP(int sockfd, void* buf, size_t len)
{
  int recvd = 0;
  struct timeval tv;
  struct sockaddr theirs;
  socklen_t tlen = sizeof(theirs);
  fd_set rfds;

  // Give exactly 2 seconds
  tv.tv_sec  = 2;
  tv.tv_usec = 0;

  FD_ZERO(&rfds);
  FD_SET(sockfd, &rfds);

  if(select(sockfd + 1, &rfds, NULL, NULL, &tv) < 0)
    return -1;

  if(FD_ISSET(sockfd, &rfds))
    recvd = recvfrom(sockfd, buf, len - 1, 0, &theirs, &tlen);

  ((char*)buf)[recvd] = '\0'; // Null terminate for good measure

  return recvd;
}

