/**
 * router.c
 *
 * Entry point for router code - CS/ECE438 MP2 FA12
 *
 * Author: Dennis J. McWherter, Jr. (dmcwhe2)
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "networking.h"
#include "protocol.h"

typedef struct
{
  router_t* router;
  int sockfd;
} udp_dat_t;

// Forward decl
void* udp_thread(void* data);

int main(int argc, char* argv[])
{
  int hostfd = 0;
  int serverfd = 0;
  router_t router;
  pthread_t thread;
  udp_dat_t udp_data;

  if(argc < 4) {
    fprintf(stderr, "Usage: %s [hostname] [managerport] [listenport]\n", argv[0]);
    return -1;
  }

  serverfd = initUDPServer(argv[3]);
  if(serverfd < 0) {
    fprintf(stderr, "Could not init UDP server. Exiting...\n");
    return -1;
  }

  hostfd = initTCPConnect(argv[1], argv[2]);
  if(hostfd < 0) {
    close(serverfd);
    fprintf(stderr, "Could not connect to manager. Exiting...\n");
    return -1;
  }

  // Only localhost for this mp?
  init_router(&router, "localhost", argv[3]);

  if(doHandshake(hostfd, &router) != 0) {
    fprintf(stderr, "Handshake failed.\n");
    destroy_router(&router);
    close(hostfd);
    return -1;
  }

  printf("manager replied with address %d\n", router.id);

  // Enable logging
  setLogging(1, hostfd, &router);

  // Start UDP server
  udp_data.sockfd = serverfd;
  udp_data.router = &router;
  pthread_create(&thread, NULL, udp_thread, (void*)&udp_data);

  // Control TCP
  tcp_control(hostfd, &router);

  pthread_join(thread, NULL);

  close(hostfd);
  close(serverfd);
  destroy_router(&router);

  return 0;
}

/**
 * UDP Controller thread
 */
void* udp_thread(void* data)
{
  udp_dat_t* udp_data = (udp_dat_t*)data;

  udp_control(udp_data->sockfd, udp_data->router);

  pthread_exit(0);
}

