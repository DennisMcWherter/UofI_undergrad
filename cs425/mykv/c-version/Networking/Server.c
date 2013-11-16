/**
 * Server.c
 *
 * Server implementation
 */

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "HandleClient.h"
#include "Protocol.h"
#include "Server.h"
#include "Sockets.h"

void* handleClientThread(void* data)
{
  ServerConfig* config = (ServerConfig*)data;
  int sockfd = 0;
  int newConnection = 0;
  struct sockaddr_in addr;
  socklen_t addrSize = sizeof(struct sockaddr_in);

  sockfd = getServerSock(config->bindIP, config->clientPort);

  while((newConnection = accept(sockfd, (struct sockaddr*)&addr, &addrSize)) != 0) {
    /* Keep using this socket until told otherwise */
    while(newClient(newConnection)) {
      /* process the request buffer */
    }
  }

  close(sockfd);

  pthread_exit(0);
}

void* startServer(void* data)
{
  ServerConfig* config = (ServerConfig*)data;
  TrafficStats* traffic = malloc(sizeof(TrafficStats));
  int server = 0;
  int newConnection = 0;
  pthread_t clientThread;
  void* res;

  /** Init traffic */
  traffic->bytesReceived = 0;
  traffic->bytesSent = 0;

  server = getServerSock(config->bindIP, config->bindPort);

  if(server == -1)
    pthread_exit(traffic);

  pthread_create(&clientThread, NULL, handleClientThread, data); 
 
  while(1) {
    newConnection = waitForClient(server);
    respondToClient(newConnection, traffic, config->fail_rate);
  }

  pthread_join(clientThread, &res);

  close(server);

  pthread_exit(traffic);
}

int waitForClient(int server)
{
  char ip[INET_ADDRSTRLEN];
  int newConnection = -1;
  struct sockaddr_in cli;
  struct sockaddr_in mine;
  socklen_t clilen  = sizeof(struct sockaddr);
  socklen_t minelen = sizeof(struct sockaddr);
  
  while((newConnection = accept(server, (struct sockaddr*)&cli, &clilen)) == -1);
  
  if(newConnection != -1) {
    getsockname(newConnection, (struct sockaddr*)&mine, &minelen);
    inet_ntop(AF_INET, &cli.sin_addr, ip, INET_ADDRSTRLEN);
    printf("CONNECTION established: %s:%u\n", ip, ntohs(mine.sin_port));
  }

  return newConnection;
}

void respondToClient(int sock, TrafficStats* traffic, int fail_rate)
{
  fd_set fds;
  char input = SRV_READY_MSG; /* Single byte output */

  if(sock == -1)
    return;

  FD_ZERO(&fds);
  FD_SET(sock, &fds);
  if(select(sock+1, NULL, &fds, NULL, NULL) != -1 && FD_ISSET(sock, &fds))
    sendData(sock, (void*)&input, sizeof(input));
  
  while(1) {
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
   
    if(select(sock+1, &fds, NULL, NULL, NULL) != 1 || !FD_ISSET(sock, &fds))
      break; /* Should not happen if no failures */

    recvData(sock, &input, sizeof(input));

    processRequest(sock, input);
  }

  close(sock);
}

void processRequest(int sock, char req)
{
  switch(req)
  {
    case SRV_READY_MSG:
    default:
      /* Do nothing */
      break;
  }
  printf("Handling: %d\n", (int)req);
}

