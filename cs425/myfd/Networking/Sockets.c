/**
 * Sockets.c
 *
 * Sockets implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "Sockets.h"

int getServerSock(const char* addr, unsigned port)
{
  struct addrinfo hints;
  struct addrinfo* servinfo = NULL;
  int sockfd = 0;
  char portStr[33]; /** Sufficiently large buffer */
  int enable = 1;

  snprintf(portStr, sizeof(portStr), "%d", port);

  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE;

  if(getaddrinfo(addr, portStr, &hints, &servinfo) != 0) {
    if(servinfo != NULL)
      freeaddrinfo(servinfo);
    return -1;
  }

  sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

  /** Bind to port */
  if(bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) != 0) {
    if(servinfo != NULL)
      freeaddrinfo(servinfo);
    close(sockfd);
    return -1;
  }

  if(listen(sockfd, MAX_BACKLOG) != 0) {
    if(servinfo != NULL)
      freeaddrinfo(servinfo);
    close(sockfd);
    return -1;
  }

  freeaddrinfo(servinfo);

  return sockfd;
}

int connectToServer(const char* addr, unsigned port)
{
  struct addrinfo hints;
  struct addrinfo* res = NULL;
  int sockfd = 0;
  char portStr[33];
  struct sockaddr_in connInfo;
  socklen_t connLen;
  int enable = 0;

  snprintf(portStr, sizeof(portStr), "%d", port);

  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  
  if(getaddrinfo(addr, portStr, &hints, &res) != 0) {
    if(res != NULL)
      freeaddrinfo(res);
    return -1;
  }

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

  if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    if(res != NULL)
      freeaddrinfo(res);
    close(sockfd);
    return -1;
  }

  freeaddrinfo(res);

  return sockfd;
}

int sendData(int sockfd, void* data, int len)
{
  if(!isBigEndian())
    switchEndian(data, len);
  return write(sockfd, data, len);
}

int recvData(int sockfd, void* data, int len)
{
  int ret = read(sockfd, data, len);
  if(!isBigEndian())
    switchEndian(data, ret);
  return ret;
}

int isBigEndian()
{
  return !((0xff & 0xdeadbeef) ^ 0xde);
}

void switchEndian(void* data, unsigned size)
{
  unsigned end = size - 1;
  int i = 0;
  char tmp;
  char* newPos  = NULL;
  char* currPos = NULL;

  if(data == NULL || size == UINT_MAX)
    return;

  for(i = 0 ; i < size/2 ; ++i) {
    currPos = (data + i);
    newPos  = (data + (end - i));

    if(currPos == newPos)
      break;

    tmp = *newPos;
    *newPos &= (~(0xff) << (end - i));
    *newPos |= *currPos;
    *currPos &= 0x00;
    *currPos |= tmp;
  }
}

