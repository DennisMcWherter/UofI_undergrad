/**
 * protocol.c
 *
 * Implementation of protocol methods
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#include "protocol.h"
#include "networking.h"
#include "ConcurrentQueue.h"

#define MAX_BUFFER 65537 // Max size of UDP packet + 1 for null byte
#define MAX_TTL 1 // This should be sufficient?

// Forward decl
static void insert_neighbor(router_t* router, network_t* neighbor);
static void update_neighbor(router_t* router, int id, int cost);
static network_t* find_neighbor(router_t* router, int id);
static network_t* find_route(router_t* router, int id);

/** Manager protocol */
/**
 * Initiate the handshake between router
 * and the manager
 *
 * @param sockfd    Open connection to the manager
 * @param router    Router object
 *
 * @return  0 on success. Not 0 otherwise.
 */
int doHandshake(int sockfd, router_t* router)
{
  static const char* helo = "HELO\n";
  static const char* ok = "OK";
  static const char* ready = "READY\n";
  static const char* neigh = "NEIGH?\n";
  char hostinfo[1024];
  char hostname[256];
  char port[64];
  char* resp = NULL;
  int cost = 0;
  int addr = 0;
  int txed = 0;
  int id = 0;
  int off = 0;
  int toff = 0;
  link_t link;
  network_t* neighbor = NULL;

  if(router == NULL)
    return -1; // Error

  // Send HELO msg
  txed = sendTCP(sockfd, helo, strlen(helo));

  if(txed <= 0) {
    perror("send");
    return -1;
  }

  // Get server response
  resp = recvTCP(sockfd, "\n", 256, -1);

  // Extract the id from the response
  if(resp == NULL || 1 != sscanf(resp, "ADDR %d", &id)) {
    fprintf(stderr, "Error: server sent bad response (Received: %s, Expected: %s)\n", resp, "ADDR _id_");
    free(resp);
    return -1;
  }

  router->id = id;

  free(resp);

  // Send host information
  sprintf(hostinfo, "HOST %s %s\n", router->host, router->port);
  txed = sendTCP(sockfd, hostinfo, strlen(hostinfo));
  
  if(txed <= 0) {
    perror("send");
    return -1;
  }

  // Next msg should be "OK"
  resp = recvTCP(sockfd, "\n", 32, -1);

  if(resp == NULL || strncmp(resp, ok, strlen(ok)) != 0) {
    fprintf(stderr, "Error: server sent bad response (Received: %s, Expected: %s)\n", resp, ok);
    free(resp);
    return -1;
  }

  free(resp);

  // Try to get our neighbors
  txed = sendTCP(sockfd, neigh, strlen(neigh));

  if(txed <= 0) {
    perror("send");
    return -1;
  }

  // Wait to recv our response about neighbors
  resp = recvTCP(sockfd, "DONE\n", -1, -1);

  // Store the data
  while(4 == sscanf(resp + toff, "NEIGH %d %s %s %d\n%n", &addr, hostname, port, &cost, &off)) {
    neighbor = malloc(sizeof(network_t));
    neighbor->id = addr;
    neighbor->host = malloc(strlen(hostname) + 1);
    neighbor->port = malloc(strlen(port) + 1);
    neighbor->cost = -1; // No neighbor links known at this point
    neighbor->directcost = cost; // Cost of taking link to neighbor
    neighbor->nexthop = NULL; // If nexthop = NULL then any value for cost (other than -1) is invalid
    
    strncpy(neighbor->host, hostname, strlen(hostname));
    strncpy(neighbor->port, port, strlen(port));

    neighbor->host[strlen(hostname)] = '\0';
    neighbor->port[strlen(port)] = '\0';

    insert_neighbor(router, neighbor);

    update_edge(&router->graph, router->id, addr, cost); // Add this to the graph

    link.end1 = router->id;
    link.end2 = addr;
    link.cost = cost;
    push_queue(&router->confirmed, &link, sizeof(link));

    toff += off;
  }
  
  // NOTE: If we made it to this point we already found the "DONE\n" message
  // it was truncated/used to terminate the string.

  free(resp);

  // Tell the server we are ready
  txed = sendTCP(sockfd, ready, strlen(ready)); 

  if(txed <= 0) {
    perror("send");
    return -1;
  }

  // Should receive another "OK" message
  resp = recvTCP(sockfd, "\n", 32, -1);

  if(resp == NULL || strncmp(resp, ok, strlen(ok)) != 0) {
    fprintf(stderr, "Error: server sent bad response (Received: %s, Expected: %s)\n", resp, ok);
    free(resp);
    return -1;
  }

  free(resp);

  return 0;
}

/**
 * Set logging on or off
 *
 * @param log     Enabling or disabling logging.
 *                  * 0 = disabled
 *                  * anthing else = enabled
 * @param sockfd  Open TCP socket to manager
 * @param router  Router object
 */
void setLogging(int log, int sockfd, router_t* router)
{
  static const char* on = "LOG ON";
  static const char* off = "LOG OFF";
  const char* str = NULL;
  char msg[32];
  char* in = NULL;
  int len = 0;

  if(router == NULL)
    return;

  pthread_mutex_lock(&router->lock);
  router->log = log;
  pthread_mutex_unlock(&router->lock);

  if(log != 0)
    sprintf(msg, "LOG ON\n");
  else
    sprintf(msg, "LOG OFF\n");

  if(sendTCP(sockfd, msg, strlen(msg)) <= 0) {
    perror("send");
    return;
  }

  in = recvTCP(sockfd, "\n", 16, -1);

  str = (log == 0) ? off : on;
  len = (log == 0) ? strlen(off) : strlen(on);

  if(strncmp(in, str, len) != 0)
    fprintf(stderr, "Server manager could not enable logging.\n");

  free(in);
}

/**
 * Notify manager of UDP events
 *
 * @param sockfd    Open socket to manager
 * @param router    Router object
 */
void notifyManager(int sockfd, router_t* router)
{
  char* top = NULL;
  char* recv = NULL;
  char first[256];
  int len = 0;

  if(router == NULL)
    return;

  // Check the received queue
  while(isEmpty(&router->notify) > 0) {
    top = (char*)pop_queue(&router->notify);
    len = *((int*)top);
    sscanf(top + 4, "%255s", first);
    //printf("Sending: %s (%d)\n", top+4, len);
    sendTCP(sockfd, top + 4, len);
    free(top);
    recv = recvTCP(sockfd, "\n", -1, -1);
    if(strncmp(first, "LOG", strlen("LOG")) == 0) {
      if(strncmp(recv, "LOG OK", strlen("LOG OK")) != 0) {
        //printf("Error receiving LOG OK (instead: %s)\n", recv);
      }
    } else {
      if(strncmp(recv, "OK", strlen("OK")) != 0) {
        //printf("Error receiving OK (instead: %s)\n", recv);
      }
    }
    free(recv);
  }
  // Let the other threads know we are done
  pthread_mutex_lock(&router->lock);
  pthread_cond_broadcast(&router->qsync);
  pthread_mutex_unlock(&router->lock);
}


/**
 * Main TCP controller
 *
 * @param sockfd    Open TCP socket
 * @param router    The router structure
 */
void tcp_control(int sockfd, router_t* router)
{
  char out[MAX_BUFFER];
  char first[256];
  char* in = NULL;
  int end = 0;
  int off = 0;
  int fid = 0, sid = 0;
  int cost = 0;
  int txed = 0;
  link_t link;

  if(router == NULL)
    return;

  while(!end) {
    in = recvTCP(sockfd, "\n", -1, 1);

    memset(out, 0, sizeof(out));

    if(in != NULL) {
      // Extract first word
      sscanf(in, "%255s%n", first, &off);
      first[off] = '\0';

      // Figure out what to do
      if(strncmp(first, "END", strlen("END")) == 0) {
        end = 1;
        pthread_mutex_lock(&router->lock);
        router->done = 1; // Let UDP server know to quit
        pthread_mutex_unlock(&router->lock);
        sprintf(out, "BYE\n");
      } else if(strncmp(first, "LINKCOST", strlen("LINKCOST")) == 0) {
        if(3 != sscanf(in, "LINKCOST %d %d %d", &fid, &sid, &cost)) {
         fprintf(stderr, "Problem interpreting LINKCOST\n");
       } else {
         //printf("Received: %s\n", in);
         link.end1 = router->id;
         link.cost = cost;
         if(router->id != fid) {
           update_edge(&router->graph, router->id, fid, cost);
           link.end2 = fid;
           //update_neighbor(router, fid, cost);
         } else {
           update_edge(&router->graph, router->id, sid, cost);
           link.end2 = sid;
           //update_neighbor(router, sid, cost);
         }
         sprintf(out, "COST %d OK\n", cost);
         // TODO: Add this to the confirmed list so it begins to 
         // This waits to make sure we started the update..
         // this may be bad?
         pthread_mutex_lock(&router->lock);
         push_queue(&router->confirmed, &link, sizeof(link));
         pthread_cond_wait(&router->qsync, &router->lock);
         pthread_mutex_unlock(&router->lock);
        }
      } else {
        //printf("Don't know how to handle: %s\n", in);
        out[0] = '\0'; // Don't know what to do - received something we don't handle
      }
      free(in);
      in = NULL;

      // Send our response to the manager
      txed = sendTCP(sockfd, out, strlen(out));

      if(txed <= 0 && strlen(out) != txed) {
        perror("error sending");
        break;
      }
    }

    // Let the manager know if we did anything through UDP
    notifyManager(sockfd, router);
  }
}

/** UDP Server Manager */
/**
 * Forward or receive a data message
 *
 * @param msg     Data message
 * @param router  Router object
 */
void udp_data_msg(const char* msg, size_t len, router_t* router)
{
  int dst = 0;
  char* out = NULL;
  int copied = 0;
  network_t* nexthop = NULL;
  struct addrinfo hints, *sinfo;
  int sockfd = 0;
  char* backlog = NULL;

  if(msg == NULL || router == NULL)
    return;

  // UDP
  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  // msg[0] = TTL byte
  // msg[1] = Server code byte
  dst = (msg[2] << 8) | msg[3];

  out = malloc((128 + len) * sizeof(char)); // Sufficiently large buffer
  //printf("handling packet...\n");

  if(dst == router->id) {
    //printf("Received something\n");
    copied = sprintf(out + 4, "RECEIVED %s\n", msg + 4);
    if(copied > 0) {
      // Store length in first 4 bytes
      *((int*)out) = copied;
      push_queue(&router->notify, out, copied + 4);
    }
  } else { // Forward the packet
    //printf("Trying to forward...\n");
    nexthop = find_route(router, dst);
    //printf("route found...\n");
    copied = (nexthop == NULL) ? 0 : sprintf(out + 4, "LOG FWD %d %s\n", nexthop->id, msg + 4);
    
    // Funny thing - need to log with TCP before it will accept
    // that it has been sent.
    if(copied > 0) {
      *((int*)out) = copied;
      pthread_mutex_lock(&router->lock);
      push_queue(&router->notify, out, copied + 4);
      pthread_cond_wait(&router->qsync, &router->lock); // Wait until queue has been processed
      pthread_mutex_unlock(&router->lock);
    }

    if(nexthop != NULL) {
      //printf("Forwarding...\n");
      getaddrinfo(nexthop->host, nexthop->port, &hints, &sinfo);

      sockfd = socket(sinfo->ai_family, sinfo->ai_socktype, sinfo->ai_protocol);
      if(sockfd == -1) {
        //printf("Could not open a UDP socket\n");
        return;
      }

      sendto(sockfd, msg + 1, len - 1, 0, sinfo->ai_addr, sinfo->ai_addrlen);

      freeaddrinfo(sinfo);
      close(sockfd);
    } else { // Backlog and wait until later (we should eventually get a route?)
    //printf("Backlogging...\n");
      backlog = malloc(sizeof(int) + len);
      memcpy(backlog + 4, msg, len);
      backlog[4] = msg[0] + 1; // Increment TTL
      *((int*)backlog) = len;
      push_queue(&router->backlog, backlog, len + sizeof(int));
      free(backlog);
    }
  }
  free(out);
}

/**
 * Update the routes to routers
 *
 * @param buf     Received buffer msg
 * @param len     Number of bytes received
 * @param router  Router object
 */
void udp_update_routes(const char* buf, size_t len, router_t* router)
{
  int dest = 0;
  int srchop = 0;
  int cost = 0;
  int dist = 0;
  link_t link;

  if(router == NULL || len != 9)
    return;

  // Datagram layout (9 bytes total):
  // Control message (1 byte) [0]
  // nexthop (2 byte) [1-2]
  // dest (2 byte) [3-4]
  // cost (4 byte) [5-8]
  srchop = (buf[1] << 8) | buf[2];
  dest   = (buf[3] << 8) | buf[4];
  cost   = *((int*)(buf+5));

  //printf("Received: %d -> %d (%d)\n", srchop, dest, cost);

  // Only manager will update my routes
  if(srchop != router->id && dest != router->id) {
    pthread_mutex_lock(&router->lock);
    dist = distance_between(&router->graph, srchop, dest);
    if(dist != cost) {
      update_edge(&router->graph, srchop, dest, cost);
      // TODO: Let the world know (i.e. add to confirmed list)
      link.end1 = srchop;
      link.end2 = dest;
      link.cost = cost;
      push_queue(&router->confirmed, &link, sizeof(link));
    }
    pthread_mutex_unlock(&router->lock);
  }
}

/**
 * Send routing tables to neighbors
 *
 * @param router    Router object
 */
void send_routes(router_t* router)
{
  network_t** direct = NULL;
  network_t* next = NULL;
  node_t* node = NULL;
  edge_t* edge = NULL;
  int neigh = 0;
  int endpoint = 0;
  int i = 0, j = 0;
  int sockfd = 0;
  char dgram[9];
  struct addrinfo hints, *sinfo = NULL;
  link_t* link = NULL;

  if(router == NULL)
    return;

  // Hints for UDP (IPv4 only)
  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd == -1) {
    //printf("Could not open UDP socket\n");
    return;
  }

  // Send our graph
  pthread_mutex_lock(&router->lock);
  node = find_node(&router->graph, router->id);
  pthread_mutex_unlock(&router->lock);

  while(isEmpty(&router->confirmed) > 0) {
    link = (link_t*)pop_queue(&router->confirmed);
    for(i = 0 ; i < node->edgecount ; ++i) {
      edge = node->edges[i];
      endpoint = (edge->endpoint1 == router->id) ? edge->endpoint2 : edge->endpoint1;
      // Pack it up
      dgram[0] = 2;
      dgram[1] = (char)((link->end1 & (int)0xff00) >> 8);
      dgram[2] = (char)((link->end1 & (int)0x00ff));
      dgram[3] = (char)((link->end2 & (int)0xff00) >> 8);
      dgram[4] = (char)((link->end2 & (int)0x00ff));
      *((int*)(dgram+5)) = link->cost;

      // Send to all neighbors
      for(j = 0 ; j < node->edgecount ; ++j) {
        neigh = (node->edges[j]->p2 == node) ? ((node_t*)node->edges[j]->p1)->id : ((node_t*)node->edges[j]->p2)->id;
        if(endpoint == neigh)
          continue;
        next = find_neighbor(router, neigh);
        getaddrinfo(next->host, next->port, &hints, &sinfo);

        //printf("Sending route: %d -> %d (%d)\n", router->id, endpoint, edge->weight);

        sendto(sockfd, dgram, sizeof(dgram), 0, sinfo->ai_addr, sinfo->ai_addrlen);
    
        if(sinfo != NULL) {
          freeaddrinfo(sinfo);
          sinfo = NULL;
        }
      }
    }
    free(link);
  }

  // TODO: This is probably useless
  pthread_mutex_lock(&router->lock);
  pthread_cond_broadcast(&router->qsync);
  pthread_mutex_unlock(&router->lock);

  close(sockfd);
  free(direct);
}

/**
 * Try to deliver backlog messages
 *
 * @param router    Router object
 */
void send_backlog(router_t* router)
{
  char** blog_cont = NULL;
  char* drop = NULL;
  int blogsize = 24;
  int total = 0;
  int i = 0;
  int len = 0;
  int ttl = 0;

  if(router == NULL)
    return;

  blog_cont = malloc(blogsize * sizeof(char*));

  pthread_mutex_lock(&router->lock);

  while(isEmpty(&router->backlog) > 0) {
    if(total >= blogsize) {
      blogsize *= 2;
      blog_cont = realloc(blog_cont, blogsize * sizeof(char*));
    }
    blog_cont[total++] = (char*)pop_queue(&router->backlog);
  }

  pthread_mutex_unlock(&router->lock);

  for(i = 0 ; i < total ; ++i) {
    ttl = blog_cont[i][4];
    if(ttl < MAX_TTL) {
      len = *((int*)blog_cont[i]);
      udp_data_msg(blog_cont[i]+4, len, router);
      //printf("Tried to send backlog: %s (len: %d)\n", blog_cont[i]+7, len);
    } else {
      // Need to drop
      //printf("Drop packet\n");
      drop = malloc(128 + len); // Sufficiently large
      len = sprintf(drop + 4, "DROP %s\n", blog_cont[i] + 8);
      *((int*)drop) = len;
      push_queue(&router->notify, drop, len + sizeof(int));
      //printf("Drop msg: %s (%d/%d)", drop + 4, len, *((int*)drop));
      free(drop);
      drop = NULL;
    }
    free(blog_cont[i]);
  }

  pthread_mutex_lock(&router->lock);
  pthread_cond_signal(&router->qsync);
  pthread_mutex_unlock(&router->lock);

  free(blog_cont);
}

/**
 * UDP Control method
 *
 * @param sockfd    Open socket to UDP server
 * @param router    Router object
 */
void udp_control(int sockfd, router_t* router)
{
  int done = 0;
  int recvd = 0;
  char buf[MAX_BUFFER + 1];

  if(router == NULL)
    return;

  while(!done) {
    buf[0] = 0; // TTL byte
    buf[1] = 0; // Reset for good measure
    recvd = recvUDP(sockfd, buf + 1, sizeof(buf) - 1);

    if(recvd > 0) {
      switch(buf[1]) {
        case 1: // Data message
          udp_data_msg(buf, recvd + 1, router);
          break;
        case 2: // Control message to update routes
          udp_update_routes(buf + 1, recvd, router);
        default:
          // Unknown case
          break;
      }
    } else if(recvd < 0) {
      perror("UDP receive"); // This may be a synchronization issue
    }

    send_routes(router);
    if(buf[1] != 2) // This is to protect the TTL while we update the graph
      send_backlog(router);

    pthread_mutex_lock(&router->lock);
    done = router->done;
    pthread_mutex_unlock(&router->lock);
  }
}

/** General methods */
/**
 * Initialize the router structure
 *
 * @param router    The router structure to initialize
 * @param host      The host of the router
 * @param port      The port of the router
 */
void init_router(router_t* router, const char* host, const char* port)
{
  if(router == NULL)
    return;

  router->id = -1; // Not set
  router->host = malloc((strlen(host) + 1) * sizeof(char));
  router->port = malloc((strlen(port) + 1) * sizeof(char));
  router->neighbors = NULL; // None yet
  router->done = 0;

  strncpy(router->host, host, strlen(host));
  strncpy(router->port, port, strlen(port));
  router->host[strlen(host)] = '\0';
  router->port[strlen(port)] = '\0';

  init_queue(&router->notify);
  init_queue(&router->backlog);
  init_queue(&router->confirmed);
  init_graph(&router->graph);
  pthread_mutex_init(&router->lock, NULL);
  pthread_cond_init(&router->qsync, NULL);
}

/**
 * Destroy and cleanup a router
 *
 * @param router    The router structure to destroy
 */
void destroy_router(router_t* router)
{
  network_t* next = NULL;
  network_t* tmp = NULL;

  if(router == NULL)
    return;

  free(router->host);
  free(router->port);
  destroy_queue(&router->notify);
  destroy_queue(&router->backlog);
  destroy_queue(&router->confirmed);
  destroy_graph(&router->graph);
  pthread_mutex_destroy(&router->lock);
  pthread_cond_destroy(&router->qsync);

  next = router->neighbors;
  while(next != NULL) {
    tmp = next;
    next = next->next;
    free(tmp->host);
    free(tmp->port);
    free(tmp);
  }
}

/**
 * Insert neighbor
 *
 * @param router    Router object
 * @param neighbor  Neighbor node to insert into router's network
 */
static void insert_neighbor(router_t* router, network_t* neighbor)
{
  network_t* next = NULL;

  if(router == NULL || neighbor == NULL)
    return;

  // Just insert at the head
  pthread_mutex_lock(&router->lock);
  next = router->neighbors;
  router->neighbors = neighbor;
  router->neighbors->next = next;
  router->neighbors->prev = NULL;
  if(next != NULL)
    next->prev = router->neighbors;
  pthread_mutex_unlock(&router->lock);
}

/**
 * Update a neighbor link cost (this is for direct neighbors ONLY)
 *
 * @param router    Router object
 * @param id        Id of other link to update
 * @param cost      New cost to that link
 */
static void update_neighbor(router_t* router, int id, int cost)
{
  network_t* neighbor = NULL;

  if(router == NULL)
    return;

  pthread_mutex_lock(&router->lock);
  neighbor = router->neighbors;

  while(neighbor != NULL) {
    if(neighbor->id == id) {
      neighbor->directcost = cost;
      break;
    }
    neighbor = neighbor->next;
  }
  pthread_mutex_unlock(&router->lock);
}

/**
 * Find a neighbor with a given id in the routing table
 *
 * @param router    Router object
 * @param id        Id to find
 * @return  A pointer to the neighbor otherwise NULL if none exists.
 */
static network_t* find_neighbor(router_t* router, int id)
{
  network_t* next = NULL;

  if(router == NULL)
    return NULL;

  pthread_mutex_lock(&router->lock);
  next = router->neighbors;
  while(next != NULL) {
    if(next->id == id)
      break;
    next = next->next;
  }
  pthread_mutex_unlock(&router->lock);

  return next;
}

/**
 * Find the best route to a given id
 *
 * @param router    Router object
 * @param target    Destination id
 * @return  The best next-hop to destination. NULL if unknown.
 */
static network_t* find_route(router_t* router, int target)
{
  network_t* route = NULL;
  int nexthop = 0;

  if(router == NULL)
    return NULL;

  nexthop = shortest_path(&router->graph, router->id, target);
  //printf("shortest path: %d\n", nexthop);
  if(nexthop != -1)
    route = find_neighbor(router, nexthop);
  //printf("neighbor\n");

  return route;
}

