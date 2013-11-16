/**
 * Server.h
 *
 * Server code
 */

#ifndef NETWORKING_SERVER_H__
#define NETWORKING_SERVER_H__

#include "../Config/Config.h"
#include "Sockets.h"

/** Begin useful stuff */
/**
 * Main server routine (for pthreads)
 *
 * @param data  Should be a ServerConfig struct
 * @return  Number of bytes sent/received
 */
void* startServer(void* data);

/**
 * Wait for a client to connect
 *
 * @param server  Server socket to wait for client on
 * @return  A valid file descriptor of the new client.
 */
int waitForClient(int server);

/**
 * Respond client requests
 *
 * @param sock      Valid socket to current client
 * @param traffic   Traffic statistics pointer
 * @param fail_rate ***DEBUG ONLY*** Set the packet drop rate
 * @return  void
 */
void respondToClient(int sock, TrafficStats* traffic, int fail_rate);

/**
 * Process a server request
 *
 * @param sock  Open socket to neighboring server
 * @param req   Request message
 * @return  void
 */
void processRequest(int sock, char req);

/**
 * Server thread to listen for clients
 *
 * @param data  ServerConfig struct
 * @return NULL
 */
void* handleClientThread(void* data);

#endif /** NETWORKING_SERVER_H__ */

