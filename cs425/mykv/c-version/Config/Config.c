/**
 * Config.c
 *
 * Implementation of configuration
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Config.h"
#include "ConfigDefs.h"
#include "../util.h"

int loadConfiguration(ServerConfig* config, const char* file)
{
  int lineNo = -1;
  char msg[CONFIG_ERRMSG_SIZE] = { 0 };

  initServerConfig(config);

  if(loadConfig(config, file, &lineNo, msg) == 0) { /** Error */
    fprintf(stderr, "Error: %s on line %d\n", msg, lineNo);
    return 0;
  }

  return 1;
}

void initServerConfig(ServerConfig* config)
{
  if(config == NULL)
    return;

  config->numKeys = 1000000; /* 1 million keys by default */
  config->numServers = 1;
  config->fail_rate = -1;
  config->serverId = 0;
  config->serverSecret = NULL;
  config->bindIP = NULL;
  config->connections = NULL;
  config->numConnections = 0;
  config->clientPort = 11111;
  config->bindPort = 1337;
  config->currentConnection = 0;
  config->totalConnections = 2;
  config->connections = malloc(config->totalConnections * sizeof(ConnectionConfig));
  memset(config->connections, 0, config->totalConnections * sizeof(ConnectionConfig));
  pthread_mutex_init(&config->mutex, NULL);
}

void destroyServerConfig(ServerConfig* config)
{
  int i = 0;

  if(config == NULL)
    return;

  if(config->serverSecret != NULL)
    free(config->serverSecret);

  if(config->bindIP != NULL)
    free(config->bindIP);

  if(config->connections != NULL) {
    for(i = 0 ; i < config->numConnections ; ++i) {
      if(config->connections[i].serverIP != NULL)
        free(config->connections[i].serverIP);
      if(config->connections[i].serverSecret != NULL)
        free(config->connections[i].serverSecret);
    }
    free(config->connections);
  }

  pthread_mutex_destroy(&config->mutex);
}

/** Internal methods */

int loadConfig(ServerConfig* config, const char* file, int* line, char msg[CONFIG_ERRMSG_SIZE])
{
  FILE* fp = NULL;
  char* currLine = NULL;
  ParserState state = VANILLA;

  if(config == NULL) {
    strncpy(msg, CONFIG_ERR_NOCONF, strlen(CONFIG_ERR_NOCONF));
    return 0;
  }

  fp = fopen(file, "r");
  if(fp == NULL) {
    strncpy(msg, CONFIG_ERR_NOFILE, strlen(CONFIG_ERR_NOFILE));
    return 0;
  }

  *line = 0;

  while((currLine = readLine(fp)) != NULL) {
    (*line)++;

    state = parseLine(config, currLine, state, msg);
    free(currLine);

    if(state == FAILED) {
      fclose(fp);
      return 0;
    }
  }

  fclose(fp);

  return 1;
}

ParserState parseLine(ServerConfig* config, const char* line, ParserState state, char msg[CONFIG_ERRMSG_SIZE])
{
  ParserState ret = state;
  int info = 0;
  const char* str  = NULL;
  char* data = NULL;
  static char waiting;
  //ConnectionConfig** conns = NULL;
  ConnectionConfig* conns = config->connections;

  line = skipWhitespace(line);

  if(line == NULL || *line == '#') /** Comment lines begin with # */
    return ret;

  switch(state)
  {
    case VANILLA:
      if(waiting == '\0') {
        if((str = stristr(line, "ServerConfiguration"))) {
          waiting = '{';
          while(*str != '\0') {
            if(*str == waiting) {
              waiting = '}';
              break;
            }
            str++;
          }
          ret = SERVER;
        } else if((str = stristr(line, "ConnectionConfiguration"))) {
          waiting = '{';
          while(*str != '\0') {
            if(*str == waiting) {
              waiting = '}';
              break;
            }
            str++;
          }
          ret = CONNECTION;
        }
      }
      info = 0;
      break;
    case SERVER:
      if(waiting == '{') {
        if(!stristr(line, "}") && (stristr(line, "ServerIP") ||
              stristr(line, "ServerId") || stristr(line, "ServerSecret") ||
              stristr(line, "ServerPort"))) {
          strncpy(msg, CONFIG_ERR_EXPECTOPEN, strlen(CONFIG_ERR_EXPECTOPEN));
          ret = FAILED;
        } else if((str = stristr(line, "{"))) {
          waiting = '}';
        }
      } else {
        if(strstr(line, "}")) {
          waiting = '\0';
          ret = VANILLA;
          break;
        }

        if((str = stristr(line, "ServerIP")))
          info = 1;
        else if((str = stristr(line, "ServerSecret")))
          info = 2;
        else if((str = stristr(line, "ServerPort")))
          info = 3;
        else if((str = stristr(line, "ServerId")))
          info = 4;
        else if((str = stristr(line, "NumServers")))
          info = 5;
        else if((str = stristr(line, "ClientPort")))
          info = 6;
        
        if(!(data = extractData(str))) {
          strncpy(msg, CONFIG_ERR_UNMATCHED, strlen(CONFIG_ERR_UNMATCHED));
          ret = FAILED;
        } else {
          if(info == 1) {
            config->bindIP = data;
          } else if(info == 2) {
            config->serverSecret = data;
          } else if(info == 3) {
            config->bindPort = atoi(data);
            free(data);
          } else if(info == 4) {
            config->serverId = atoi(data);
            free(data);
          } else if(info == 5) {
            config->numServers = atoi(data);
            free(data);
          } else if(info == 6) {
            config->clientPort = atoi(data);
            free(data);
          }
        }
      }
      break;
    case CONNECTION:
      if(waiting == '{') {
        if(!stristr(line, "}") && (stristr(line, "ServerIP") || stristr(line, "ServerSecret") || stristr(line, "ServerPort"))) {
          strncpy(msg, CONFIG_ERR_EXPECTOPEN, strlen(CONFIG_ERR_EXPECTOPEN));
          ret = FAILED;
        } else if((str = strstr(line, "{"))) {
          waiting = '}';
        }
      } else {
        if(strstr(line, "}")) {
          /** Check array size */
          config->numConnections++;

          if(config->numConnections == config->totalConnections) {
            config->totalConnections *= 2;
            config->connections = realloc(conns, config->totalConnections*sizeof(ConnectionConfig));
            conns = config->connections;
            memset(config->connections+config->numConnections, 0, config->numConnections * sizeof(ConnectionConfig));
          }

          waiting = '\0';
          ret = VANILLA;
          break;
        }

        if((str = stristr(line, "ServerIP")))
          info = 1;
        else if((str = stristr(line, "ServerSecret")))
          info = 2;
        else if((str = stristr(line, "ServerPort")))
          info = 3;
        
        if(!(data = extractData(str))) {
          strncpy(msg, CONFIG_ERR_UNMATCHED, strlen(CONFIG_ERR_UNMATCHED));
          ret = FAILED;
        } else {
          if(info == 1) {
            conns[config->numConnections].serverIP = data;
            conns[config->numConnections].sock = 0;
          } else if(info == 2) {
            conns[config->numConnections].serverSecret = data;
          } else if(info == 3) {
            conns[config->numConnections].serverPort = atoi(data);
            free(data);
          }
        }
      }
      break;
    default: /** Don't know how to handle? */
      ret = FAILED;
      break;
  }

  return ret;
}

