/**
 * Sockets.h
 *
 * Socket header
 */

#ifndef NETWORKING_SOCKETS_H__
#define NETWORKING_SOCKETS_H__

/** Networking headers */
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

/** Defines */
#define MAX_BACKLOG 0x0a

/**
 * Struct which contains bytes received and sent
 */
typedef struct
{
  unsigned bytesReceived;
  unsigned bytesSent;
} TrafficStats;

/**
 * Get a server socket
 *
 * @param bind    Address to bind
 * @param port    Port to bind
 * @return  A file descriptor to an open socket.
 *  -1 on error.
 */
int getServerSock(const char* bind, unsigned port);

/**
 * Connect to a server
 *
 * @param addr    Address of server
 * @param port    Port to connect to
 * @return  An open file descriptor to the server.
 *  -1 on error.
 */
int connectToServer(const char* addr, unsigned port);

/**
 * Send a message to a server
 *
 * @param sock  Socket to send message
 * @param msg   Data to send
 * @param len   Message length
 * @return  Returns number of bytes sent.
 *    -1 on error.
 */
int sendData(int sock, void* data, int len);

/**
 * Receive a message
 *
 * @param sock    Socket to receive from
 * @param buffer  Buffer to file with data
 * @param size    Buffer size
 * @return  Number of bytes read. -1 on error.
 */
int recvData(int sock, void* buffer, int len);

/**
 * Test Endianess
 *
 * @return  0 if little endian and 1 if big endian.
 *  Not handling middle-ending.
 */
int isBigEndian();

/**
 * Switch endianess
 *  NOTE: method has artifiacts and changes
 *    data directly.
 *
 * @param data  Data to flip
 * @param size  Size of data
 */
void switchEndian(void* data, unsigned size);

#endif /** NETWORKING_SOCKETS_H__ */


