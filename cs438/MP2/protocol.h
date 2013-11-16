/**
 * protocol.h
 *
 * All protocol related functionality
 *
 * Author: Dennis J. McWherter, Jr. (dmcwhe2)
 */

#ifndef PROTOCOL_H__
#define PROTOCOL_H__

#include <pthread.h>

#include "ConcurrentQueue.h"
#include "graph.h"

/**
 * General network information
 */
typedef struct _network_t
{
  int id;
  int cost;
  int directcost;
  char* host;
  char* port;
  struct _network_t* nexthop;
  struct _network_t* next;
  struct _network_t* prev;
} network_t;

typedef struct
{
  int end1;
  int end2;
  int cost;
} link_t;

/**
 * Router information
 */
typedef struct
{
  int id;
  char* host;
  char* port;
  int log;
  int done;
  queue_t notify;
  queue_t backlog;
  queue_t confirmed;
  graph_t graph;
  network_t* neighbors;
  pthread_mutex_t lock;
  pthread_cond_t qsync;
} router_t;

// Router helper methods
void init_router(router_t* router, const char* host, const char* port);
void destroy_router(router_t* router);

// Manager protocol methods
int doHandshake(int sockfd, router_t* router); 
void setLogging(int log, int sockfd, router_t* router);
void tcp_control(int sockfd, router_t* router);
void notifyManager(int sockfd, router_t* router);

// UDP methods
void udp_control(int sockfd, router_t* router);
void udp_data_msg(const char* msg, size_t len, router_t* router);
void udp_update_routes(const char* buf, size_t len, router_t* router);
void send_routes(router_t* router);
void send_backlog(router_t* router);

#endif /** PROTOCOL_H__ */

