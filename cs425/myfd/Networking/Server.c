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

#include "Protocol.h"
#include "Server.h"
#include "Sockets.h"

void* startServer(void* data)
{
  ServerConfig* config = (ServerConfig*)data;
  TrafficStats* traffic = malloc(sizeof(TrafficStats));
  int server = 0;
  int newConnection = 0;
  int* connections = NULL;

  /** Init traffic */
  traffic->bytesReceived = 0;
  traffic->bytesSent = 0;

  server = getServerSock(config->bindIP, config->bindPort);

  if(server == -1)
    pthread_exit(traffic);
 
  while(1) {
    newConnection = waitForClient(server);
    respondToClient(newConnection, traffic, config->fail_rate);
  }

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
    printf("SEND relationship established: %s:%u\n", ip, ntohs(mine.sin_port));
  }

  return newConnection;
}

void respondToClient(int sock, TrafficStats* traffic, int fail_rate)
{
  fd_set fds;
  char output = SRV_LIVE_MSG; /* Single byte output */
  struct timeval timeout;
  time_t start = 0, end = 0;
  useconds_t sleepTime;
  struct sockaddr_in info;
  socklen_t infolen = sizeof(struct sockaddr);
  char ip[INET_ADDRSTRLEN];
  int err = 0;
  int fail = 0;
  srand(time(NULL));
  int randDrop = 0;

  if(sock == -1)
    return;

  getpeername(sock, (struct sockaddr*)&info, &infolen);
  inet_ntop(AF_INET, &info.sin_addr, ip, INET_ADDRSTRLEN);
  
  FD_ZERO(&fds);

  while(1) {
    randDrop = rand() % 99 + 0;
    FD_SET(sock, &fds);
    timeout.tv_sec  = 10; /** 10 seconds */
    timeout.tv_usec = 0; /** 0 microseconds */
    
    time(&start);
    if(select(sock+1, NULL, &fds, NULL, &timeout) != 1 || !FD_ISSET(sock, &fds))
      break;
    time(&end);

    if(randDrop < fail_rate)
      output = SRV_DROP_MSG;
    else
      output = SRV_LIVE_MSG;
    
    if((err = sendData(sock, &output, sizeof(output))) < 0)
      fail++;
    else
      fail = 0;

    if(fail > 1)
      break;
    
    if(err >= 0) {
      traffic->bytesSent += sizeof(output);
      printf("Sent %d bytes\n", traffic->bytesSent);
    }

    sleepTime = (start - end < 2) ? 3000000 : (start - end < 3) ? 1500000 : 0;
    usleep(sleepTime);
  }

  printf("Failure detected: %s:%u (RECEIVER)\n", ip, ntohs(info.sin_port));

  close(sock);
}

