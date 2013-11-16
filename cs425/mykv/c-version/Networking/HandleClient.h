/**
 * HandleClient.h
 *
 * Handle Clients
 */

#ifndef NETWORKING_HANDLECLIENT_H__
#define NETWORKING_HANDLECLIENT_H__

#include "../Config/Config.h"
#include "Sockets.h"

/**
 * Main client routine (for pthreads)
 *
 * @param sockfd  Open socket to client
 * @return  NULL
 */
void newClient(int sockfd);

/** Internal methods */
/**
 * Handle a client message
 *
 * @param sockfd  Open socket to client
 * @param input   The client's message to handle
 * @return  1 on success, 0 otherwise.
 */
int handleCliMsg(int sockfd, char input);

/**
 * Handle a find request
 *
 * @param sockfd  Open socket to client
 * @return  void
 */
void handleFind(int sockfd);

/**
 * Handle an insert request
 *
 * @param sockfd  Open socket to client
 * @return  void
 */
void handleInsert(int sockfd);

/**
 * Handle a remove request
 *
 * @param sockfd  Open socket to client
 * @return  void
 */
void handleRemove(int sockfd);

#endif /** NETWORKING_HANDLECLIENT_H__ */

