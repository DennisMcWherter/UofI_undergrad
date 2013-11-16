/**
 * main.c
 *
 * Main entry point for MyFD
 * CS425 MP1
 *
 * @author Dennis J. McWherter, Jr., Jiageng Li
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "Config/Config.h"
#include "Networking/Client.h"
#include "Networking/Server.h"

void setupSignalHandler()
{
  signal(SIGPIPE, SIG_IGN);
}

int main(int argc, char** argv)
{
  ServerConfig config;
  pthread_t server;
  pthread_t client;
  int fail = -1;

  if(argc < 2) {
    fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
    return -1;
  }


  if(!loadConfiguration(&config, argv[1])) {
    destroyServerConfig(&config);
    return -1;
  }

  if(argc >= 3) {
    fail = atoi(argv[2]);
    config.fail_rate = fail;
    printf("\n***DEBUG*** Fail rate set: %d\n", fail);
  }

  setupSignalHandler(); /* Important to ignore SIGPIPE and graceful kill */

  /** Do server stuff... */
  if(pthread_create(&server, NULL, startServer, &config)) {
    fprintf(stderr, "Failed: Could not initialize server thread...\n");
    destroyServerConfig(&config);
    return -1;
  }

  if(pthread_create(&client, NULL, startClient, &config)) {
    fprintf(stderr, "Failed: Could not initialize client thread...\n");
    pthread_kill(server, SIGTERM);
    destroyServerConfig(&config);
    return -1;
  }

  while(1);

  pthread_join(server, NULL);
  pthread_join(client, NULL);

  destroyServerConfig(&config);

  return 0;
}

