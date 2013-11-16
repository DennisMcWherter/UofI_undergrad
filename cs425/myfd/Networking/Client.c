/**
 * Client.c
 *
 * Client implementation
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Client.h"
#include "../Config/Config.h"
#include "Protocol.h"
#include "Sockets.h"

void* startClient(void* data)
{
  ServerConfig* config = (ServerConfig*)data;
  ConnectionConfig* connections = config->connections;
  TrafficStats* traffic = malloc(sizeof(TrafficStats));
  int sock = -1;
  struct sockaddr_in cli;
  socklen_t clilen = sizeof(struct sockaddr);
  char ip[INET_ADDRSTRLEN];

  /** Init traffic */
  traffic->bytesReceived = 0;
  traffic->bytesSent = 0;

  sock = joinRing(config);


  while(1) {
    getpeername(sock, (struct sockaddr*)&cli, &clilen);
    inet_ntop(AF_INET, &cli.sin_addr, ip, INET_ADDRSTRLEN);
    printf("RECV relation established: %s:%u\n", ip, ntohs(cli.sin_port));
    
    recvFromServer(sock, traffic);
    usleep(1000000); /** Pause or 1 second */
    sock = reconnect(config);
  }

  close(sock);

  pthread_exit(traffic);
}

int attemptConnect(ServerConfig* config, int node)
{
  unsigned size = config->numConnections;
  ConnectionConfig* list   = config->connections;
  ConnectionConfig* server = NULL;
  int sock = 0;

  if(size == 0 || node >= size || node < 0)
    return -1; /** No servers to try or out of bounds node */

  server = &list[node];
  sock = connectToServer(server->serverIP, server->serverPort);

  if(sock != -1)
    config->currentConnection = node;

  server->sock = sock;

  return sock;
}

int reconnect(ServerConfig* config)
{
  unsigned size = config->numConnections;
  int sock = 0;
  int i = 0;
  ConnectionConfig* list   = config->connections;
  ConnectionConfig* server = NULL;

  if(size == 0)
    return -1;

  server = &list[i];
  while((sock = attemptConnect(config, i)) == -1) {
    i = (i + 1) % size;
    usleep(1000000); /* Control yourself - 1 second wait */
  }

  return sock;
}

int joinRing(ServerConfig* config)
{
  int sock = 0;
  while((sock = attemptConnect(config, 0)) == -1);
  return sock;
}

void recvFromServer(int sock, TrafficStats* traffic)
{
  fd_set fds;
  char ip[INET_ADDRSTRLEN];
  struct sockaddr_in info;
  socklen_t infolen = sizeof(struct sockaddr);
  struct timeval timeout;
  char input;
  int recv = 0;
  int fail = 0;
  time_t start = 0, end = 0;
  int numSecs = 10;
  int runFail = 0;

  if(sock == -1)
    return;

  getpeername(sock, (struct sockaddr*)&info, &infolen);
  inet_ntop(AF_INET, &info.sin_addr, ip, INET_ADDRSTRLEN);
  
  while(1) {
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    timeout.tv_sec  = numSecs; /** 10 seconds */
    timeout.tv_usec = 0; /** 0 microseconds */

    time(&start);
    if(select(sock+1, &fds, NULL, NULL, &timeout) != 1 || !FD_ISSET(sock, &fds))
      break;

    recv = recvData(sock, &input, sizeof(char));
    if(recv == 0) {
      fail++;
      usleep(1000000);
    } else {
      fail = 0;
    }

    if(fail > 8 || runFail > 8)
      break;
    time(&end);

    if(fail == 0) {
      if(input != (char)SRV_DROP_MSG) {
        traffic->bytesReceived += recv;
        printf("Received %d bytes\n", traffic->bytesReceived);
        numSecs = 10;
        runFail = 0;
      } else {
        numSecs = 10 - (end - start);
        runFail += (end - start) + 1;
      }
    }
  }

  printf("Failure detected: %s:%u (SENDER)\n", ip, ntohs(info.sin_port));

  close(sock);
}

