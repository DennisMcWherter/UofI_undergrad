/**
 * networking.h
 *
 * All effective code which purely deals with
 * the networking internal (i.e. setting up/tearing down
 * connections, etc.)
 *
 * Author: Dennis J. McWherter, Jr (dmcwhe2)
 */

#ifndef NETWORKING_H__
#define NETWORKING_H__

// Basic functions to connect and wait for data
int initUDPServer(const char* port);
int initTCPConnect(const char* host, const char* port);

// Convenience methods
int sendTCP(int sockfd, const void* buf, size_t len);
char* recvTCP(int sockfd, const char* delim, int max, int timeout);
int recvUDP(int sockfd, void* buf, size_t len);

#endif /** NETWORKING_H__ */

