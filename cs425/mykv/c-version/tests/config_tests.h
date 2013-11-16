/**
 * config_tests.h
 *
 * Configuration testing
 */

#ifndef TESTS_CONFIG_TESTS_H__
#define TESTS_CONFIG_TESTS_H__

#include "testing.h"
#include "../Config/Config.h"

static int testInitServerConfig(char* msg)
{
  ServerConfig config;

  initServerConfig(&config);

  if(config.serverId != NULL || config.serverSecret != NULL
      || config.bindIP != NULL || config.bindPort != 1337 ||
      config.numConnections != 0 || config.connections == NULL) {
    fail(msg, "Did not properly initialize the object.");
    printf("Result: %s:%ud, %s\n", config.bindIP, config.bindPort, config.serverSecret);
    return 0;
  }

  initServerConfig(NULL); /** Crash test */

  destroyServerConfig(&config);

  return 1;
}

static int testDestroyServerConfig(char* msg)
{
  ServerConfig config;

  initServerConfig(&config);

  destroyServerConfig(&config); /** Should not crash and have no leaks */
  destroyServerConfig(NULL); /** Should not crash */

  return 1;
}

static int testLoadConfiguration(char* msg)
{
  ServerConfig config;
  FILE* fp = NULL;
  char file1[] = "test1.conf";
  char file2[] = "tests/test1.conf";
  char* loc = NULL;
  int result = 0;

  /** Try to find file first */
  if((fp = fopen(file1, "r"))) {
    loc = file1;
  } else if((fp = fopen(file2, "r"))) {
    loc = file2;
  } else {
    fail(msg, "Could not open testing configuration");
    return 0;
  }

  fclose(fp);

  /** Be sure the method itself doesn't report errors */
  result = loadConfiguration(&config, loc);

  if(result == 0) {
    fail(msg, "loadConfiguration() did not complete properly");
    return 0;
  }

  /** Check values are correct */
  if(strncmp(config.bindIP, "127.0.0.1", strlen("127.0.0.1")) ||
      strncmp(config.serverSecret, "mySecretKey123", strlen("mySecretKey123")) ||
      config.serverId != 1 || config.bindPort != 1337 || config.connections[0].serverPort != 1234 ||
      config.numServers != 4 || config.clientPort != 11112) {
    fail(msg, "Did not read in correct data values");
    destroyServerConfig(&config);
    return 0;
  }

  destroyServerConfig(&config);

  return 1;
}

#endif /** TESTS_CONFIG_TESTS_H__ */

