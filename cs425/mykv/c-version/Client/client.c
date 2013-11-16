/**
 * client.c
 *
 * Simple client to communicate with mykv server
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

#include "../Networking/Protocol.h"
#include "../Networking/Sockets.h"

#define WELCOME_MSG "Welcome to the MyKV client!\n" \
  "You are connected to %s on port %u\n" \
  "Type \"help\" for instructions.\n\n"

#define HELP_MSG "\n==========\nCommands\n==========\n\n" \
  "INSERT <id> <data>\t\tInsert specified data at specified id\n" \
  "REMOVE <id>\t\t\tRemove the entry at id\n" \
  "LOOKUP <id>\t\t\tLookup the entry at id\n" \
  "HELP\t\t\t\tDisplay this menu\n" \
  "EXIT\t\t\t\tClose connection to server and exit the client\n\n"

#define BUFFER_SIZE 1024 /* Hopefully large enough - not too worried about buffer overflow */

/**
 * Methods for sending requests
 */
void runClient(int,const char*,unsigned);
void insertId(int,unsigned,const char*);
void removeId(int,unsigned);
char* lookupId(int,unsigned);

int main(int argc, char** argv)
{
  int sockfd = -1;
  unsigned port = 0;

  if(argc != 3) {
    fprintf(stderr, "Usage: %s <server_addr> <server_port>\n", argv[0]);
    return 1;
  }

  port = atoi(argv[2]);

  sockfd = connectToServer(argv[1], port);

  if(sockfd == -1) {
    fprintf(stderr, "Could not connect to %s:%u\n", argv[1], port);
    return 1;
  }

  runClient(sockfd, argv[1], port);

  close(sockfd);

  return 0;
}

void runClient(int sockfd, const char* server, unsigned port)
{
  char input[BUFFER_SIZE];
  char output = CLI_EXIT_MSG;
  char* idStr = NULL;
  char* data = NULL;
  unsigned id = 0;
  unsigned countId = 0;
  int numDigits = 0;
  fd_set fds;
  
  if(sockfd == -1)
    return;

  FD_ZERO(&fds);

  printf(WELCOME_MSG, server, port);

  while(1) {
    FD_SET(sockfd, &fds);

    fgets(input, BUFFER_SIZE, stdin);

    if(strncasecmp("help", input, strlen("help")) == 0) {
      printf(HELP_MSG);
    } else if(strlen(input) > 6) {
      input[6] = '\0'; /* All commands should have a space here */
      idStr = input+7;
      id = atoi(idStr);
      countId = id;

      /* Count number of char's for id */
      numDigits = 0;
      while(countId) { /* In base 10 */
        countId /= 10;
        numDigits++;
      }

      data = idStr + numDigits + 1; /* If needed */

      /* Determine what we want to do */
      if(strncasecmp(input, "insert", strlen("insert")) == 0) {
        output = CLI_INS_MSG;
      } else if(strncasecmp(input, "remove", strlen("remove")) == 0) {
        output = CLI_REM_MSG;
      } else if(strncasecmp(input, "lookup", strlen("lookup")) == 0) {
        output = CLI_FIND_MSG;
      }

      select(sockfd+1, NULL, &fds, NULL, NULL);
      sendData(sockfd, &output, sizeof(output));

      switch(output)
      {
        default:
          /* Do nothing */
          break;
        case CLI_INS_MSG:
          insertId(sockfd, id, data);
          break;
        case CLI_REM_MSG:
          removeId(sockfd, id);
          break;
        case CLI_FIND_MSG:
          /* Need to actually display this information if valid */
          lookupId(sockfd, id);
          break;
      }

    } else if(strncasecmp("exit", input, strlen("exit")) == 0) {
      output = CLI_EXIT_MSG;
      sendData(sockfd, &output, sizeof(output));
      break;
    } else {
      fprintf(stderr, "Unknown command\n");
    }
  }
}

void insertId(int sockfd, unsigned id, const char* data)
{
  fd_set fds;
  int size = strlen(data)+1;

  printf("Inserting: db[%u] = %s\n", id, data);

  FD_ZERO(&fds);
  FD_SET(sockfd, &fds);

  /* Send id */
  select(sockfd+1, NULL, &fds, NULL, NULL);
  if(sendData(sockfd, &id, sizeof(id)) == -1) {
    fprintf(stderr, "Could not send id.\n");
    return;
  }

  FD_SET(sockfd, &fds);

  /* Send the actual data */
  select(sockfd+1, NULL, &fds, NULL, NULL);
  if(sendData(sockfd, (void*)data, size) == -1)
    fprintf(stderr, "There was an error inserting data...\n");
  else /* TODO: Maybe send a success message back? */
    printf("Data inserted successfully!\n");
}

void removeId(int sockfd, unsigned id)
{
  fd_set fds;

  printf("Removing: db[%u]\n", id);

  FD_ZERO(&fds);
  FD_SET(sockfd, &fds);

  /* Send ID */
  select(sockfd+1, NULL, &fds, NULL, NULL);
  if(sendData(sockfd, &id, sizeof(id)) == -1)
    fprintf(stderr, "Could not send id to remove.\n");
  else
    printf("ID removed successfully!\n");
}

char* lookupId(int sockfd, unsigned id)
{
  fd_set fds;

  printf("Looking up: db[%u]\n", id);

  FD_ZERO(&fds);
  FD_SET(sockfd, &fds);

  /* Send ID */
  select(sockfd+1, NULL, &fds, NULL, NULL);
  if(sendData(sockfd, &id, sizeof(id)) == -1) {
    fprintf(stderr, "Could not send id to lookup.\n");
    return NULL;
  }

  FD_SET(sockfd, &fds);
  /* Try to read in the data */

  return NULL;
}

