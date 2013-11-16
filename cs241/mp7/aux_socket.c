#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "aux_socket.h"

/**
 * creates a new socket file descriptor, binds to the local port port,
 * then sets up a listening queue.
 */
int newServerSocket(const char* port) {
	int serverfd;
  struct addrinfo hints, *info;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE;

  if(getaddrinfo(NULL, port, &hints, &info) != 0) {
    freeaddrinfo(info);
    return -1;
  }

  if((serverfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol)) == -1)
    return -1;

  if(bind(serverfd, info->ai_addr, info->ai_addrlen) == -1)
    return -1;

  if(listen(serverfd, 100) == -1)
    return -1;

  freeaddrinfo(info);

	return serverfd;
}

/**
 * creates a new socket file descriptor, connects to host and port
 */

int newClientSocket(const char *host, const char *port){
	int clientfd;
  struct addrinfo hints, *info;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if(getaddrinfo(host, port, &hints, &info) != 0)
    return -1;

  if((clientfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol)) == -1)
    return -1;

  if(connect(clientfd, info->ai_addr, info->ai_addrlen) == -1)
    return -1;

  freeaddrinfo(info);

	return clientfd;
}

/**
 * accepts new connections on a server socket and returns the socket
 * file descriptor for the client.
 */
int acceptSocket(int serverFd) {
  socklen_t addrsize;
  struct sockaddr_in sa;

  addrsize = sizeof(sa);

  return accept(serverFd, (struct sockaddr*)&sa, &addrsize);
}

/**
 * closes socket file descriptor.
 */
void closeSocket(int sockfd) {
  close(sockfd);
}

