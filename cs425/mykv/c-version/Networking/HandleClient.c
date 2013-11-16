/**
 * HandleClient.c
 *
 * Handles client connections
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>

#include "HandleClient.h"
#include "Protocol.h"

void newClient(int sockfd)
{
  fd_set fds;
  char output = SRV_READY_MSG;
  char input;

  printf("Client connected\n");

  FD_ZERO(&fds);
  FD_SET(sockfd, &fds);

  select(sockfd+1, NULL, &fds, NULL, NULL);
  if(sendData(sockfd, (void*)&output, sizeof(output)) == -1) {
    close(sockfd);
    return;
  }
  
  while(1) {
    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);

    select(sockfd+1, &fds, NULL, NULL, NULL);
    if(recvData(sockfd, (void*)&input, sizeof(input)) == -1) {
      close(sockfd);
      return;
    }
    
    if(!handleCliMsg(sockfd, input)) {
      printf("Closed connection to client\n");
      close(sockfd);
      break;
    }
  }

  return;
}

int handleCliMsg(int sockfd, char input)
{
  switch(input) {
  case CLI_EXIT_MSG:
  default:
    return 0;
    break;
  case CLI_FIND_MSG:
    handleFind(sockfd);
    break;
  case CLI_INS_MSG:
    handleInsert(sockfd);
    break;
  case CLI_REM_MSG:
    handleRemove(sockfd);
    break;
  }

  return 1;
}

void handleInsert(int sockfd)
{
  fd_set fds;
  unsigned id = 0;
  unsigned chunkSize = 1024;
  unsigned currSize = 0;
  unsigned size = 2*chunkSize;
  char* data  = malloc(size);
  char* start = data;
  int read = 0;
  struct timeval timeout;

  printf("Received insert request.\n");

  FD_ZERO(&fds);
  FD_SET(sockfd, &fds);

  /* Expect the ID first */
  select(sockfd+1, &fds, NULL, NULL, NULL);
  if(recvData(sockfd, &id, sizeof(id)) == -1) {
    fprintf(stderr, "Could not receive id for inserting.\n");
    free(data);
    return;
  }

  memset(data, 0, size);
  FD_SET(sockfd, &fds);
  
  /* Read whole data segment */
  select(sockfd+1, &fds, NULL, NULL, NULL);
  while((read = recvData(sockfd, data, chunkSize)) > 0) {
    currSize += read;
    if(currSize + chunkSize >= size) {
      size *= 2;
      start = realloc(start, size);
      data  = start + currSize;
      memset(data, 0, size - currSize);
    } else {
      data += read;
    }
    
    /* Wait up to 1.25 seconds for new data */
    timeout.tv_sec  = 1;
    timeout.tv_usec = 250000;
    FD_SET(sockfd, &fds);
    select(sockfd+1, &fds, NULL, NULL, &timeout);
    if(!FD_ISSET(sockfd, &fds))
      break;
  }

  printf("Inserting data into id %u: %s\n", id, start);
  /* NOTE: Need to free start, but also want to send it to the main server thread for processing */
}

void handleFind(int sockfd)
{
  /** TODO: Implement */
  printf("Received find request.\n");
}

void handleRemove(int sockfd)
{
  /** TODO: Implement */
  printf("Received remove request.\n");
}

