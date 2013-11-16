/**
 * sockets_test.h
 *
 * Testing sockets
 */

#include "testing.h"
#include "../Networking/Sockets.h"

/** Networking */
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

/** Helper functions */
static int createSocket(const char* host, const char* port)
{
  struct addrinfo hints;
  struct addrinfo* res;
  int sock = 0;

  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  getaddrinfo(host, port, &hints, &res);

  sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  connect(sock, res->ai_addr, res->ai_addrlen);

  freeaddrinfo(res);

  return sock;
}

/** Tests */
static int testServer(char* msg)
{
  char test[] = "This work?";
  char resp[] = "Response!";
  char buffer[30];
  int child = 0;
  int sock = 0;
  int testConnect = 0;
  int status = 0;
  fd_set fds;
  struct sockaddr_in clientAddr; /* Only care about IPv4 */
  socklen_t addrSize;
  
  FD_ZERO(&fds);

  if((child = fork()) != 0) { /* Parent */
    printf("Test");
    sock = getServerSock("localhost", 5555);

    if(sock == -1) {
      fail(msg, "Could not bind to port on this address!");
      kill(child, SIGTERM);
      return 0;
    }

    testConnect = accept(sock, (struct sockaddr*)&clientAddr, &addrSize);
    
    FD_SET(testConnect, &fds);
    while(FD_ISSET(testConnect, &fds))
      select(testConnect+1, &fds, NULL, NULL, NULL);
    if(read(testConnect, buffer, sizeof(test)) != sizeof(test)) {
      fail(msg, "Did not properly receive message at the server end.");
      close(testConnect);
      close(sock);
      kill(child, SIGTERM);
      return 0;
    } else if(strncmp(buffer, test, sizeof(test))) {
    fail(msg, "Did not receive the correct message at the server end.");
      close(testConnect);
      close(sock);
      kill(child, SIGTERM);
      return 0;
    }
    FD_SET(testConnect, &fds);
    select(testConnect+1, NULL, &fds, NULL, NULL);
    write(testConnect, resp, sizeof(resp));
  } else { /* Child */
    testConnect = createSocket("localhost", "5555");
   
    if(testConnect == -1) {
      printf("Child could not establish connection!\n");
      exit(0);
    }

    FD_SET(testConnect, &fds);
    while(FD_ISSET(testConnect, &fds))
      select(testConnect+1, NULL, &fds, NULL, NULL);
    write(testConnect, test, sizeof(test));
    FD_SET(testConnect, &fds);
    while(FD_ISSET(testConnect, &fds))
      select(testConnect+1, &fds, NULL, NULL, NULL);
    if(read(testConnect, buffer, sizeof(resp)) != sizeof(resp)) {
      fail(msg, "Did not properly receive message at client end.");
      close(testConnect);
      close(sock);
      exit(0);
    } else if(strncmp(resp, buffer, sizeof(resp))) {
      fail(msg, "Did not receive the correct message at the client end.");
      close(testConnect);
      close(sock);
      exit(0);
    }
  exit(0);
  }
  waitpid(child, &status, -1);

  close(testConnect);
  close(sock);

  return 1;
}

static int testEndianess(char* msg)
{
  if(isBigEndian() == 1) {
    fail(msg, "Wrong endianess. NOTE: This test only works on little-endian machines. If you are running a big-endian machine, the function may still work.");
    return 0;
  }
  return 1;
}

static int testSwitchEndian(char* msg)
{
  unsigned test = 0xdeadbeef;
  char testchar[] = "testme";
  char testchar2[] = "andtestme2!";

  switchEndian(&test, sizeof(test));

  if(0xefbeadde != test) {
    fail(msg, "Did not switch endianess appropriately.");
    return 0;
  }

  switchEndian(&testchar, sizeof(testchar));

  if(strncmp(testchar+1, "emtset", sizeof(testchar)-1)) {
    printf("Result: %s\n", testchar);
    fail(msg, "Did not reverse string even-length properly.");
    return 0;
  }

  switchEndian(&testchar2, sizeof(testchar2));

  if(strncmp(testchar2+1, "!2emtsetdna", sizeof(testchar2)-1)) {
    fail(msg, "Did not reverse odd-length string appropriately");
    return 0;
  }

  return 1;
}


