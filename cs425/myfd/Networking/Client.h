/**
 * Client.h
 *
 * Client
 */

#ifndef NETWORKING_CLIENT_H__
#define NETWORKING_CLIENT_H__

#include "../Config/Config.h"
#include "Sockets.h"

/**
 * Main client routine (for pthreads)
 *
 * @param data  Should be a ServerConfig struct
 * @return  Number of bytes sent/received
 */
void* startClient(void* data);

/** Internal methods */
/**
 * Try to connect to servers in list
 *
 * @param config   Server configuration pointer
 * @param node     The number corresponding to the node to
 *                   attempt to connect to
 * @return  A valid socket - keep trying until a
 *  connection is established. -1 if no servers
 *  in the configuration..
 */
int attemptConnect(ServerConfig* config, int node);

/**
 * Reconnect if failure
 *
 * @param config    Server configuration pointer
 * @return  A valid socket - keeps trying until a
 *  connection is established. -1 on error.
 */
int reconnect(ServerConfig* config);

/**
 * Join the ring
 *
 * Method continues to attempt to establish a
 * connection with the first node in its list.
 * It will not advance passed this point until
 * this connection is established.
 *
 * @param config    Server configuration pointer
 *
 * @return  A valid socket to the server
 */
int joinRing(ServerConfig* config);

/**
 * Receive messages from the server
 *
 * @param sock      Valid socket to server
 * @param traffic   Traffic statistics pointer
 * @return  void
 */
void recvFromServer(int sock, TrafficStats* traffic);

#endif /** NETWORKING_CLIENT_H__ */

