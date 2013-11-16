/**
 * digraph.c
 *
 * Light-weight directed graph implementation
 *
 * @author Dennis J. McWherter, Jr.
 */

#include <stdlib.h>
#include <string.h>

#include "graph.h"

#define GRAPH_DFLT_SIZE 32
#define DFLT_EDGE_SIZE 64
#define INFINITY -1

// Forward decl
static int shortest_path_helper(graph_t* graph, node_t* src, node_t* target);

/**
 * Internal priority queue entry
 */
typedef struct
{
  node_t* node;
  int best;
  int marked;
} pqueue_elem_t;

/**
 * Find the corresponding index of a given value
 *
 * @param table   The table to look into
 * @param value   Value to find
 * @param len     Size of table
 * @return  The corresponding index. -1 if it does not exist
 */
static int findIdx(int* table, int value, size_t len)
{
  int i = 0;
  
  if(table == NULL)
    return -1;

  for(i = 0 ; i < len ; ++i)
    if(table[i] == value)
      return i;

  return -1;
}

/**
 * Find a node
 *
 * @param graph   Graph object to find node in
 * @param id      Id of the node to find
 * @return  A pointer to the corresponding node, otherwise
 *        NULL if not found
 */
static node_t* findNode(graph_t* graph, int id)
{
  int i = 0;

  if(graph == NULL)
    return NULL;

  for(i = 0 ; i < graph->nodecount ; ++i) {
    if(graph->nodes[i]->id == id)
      return graph->nodes[i];
  }

  return NULL; // Could not find
}

/**
 * Add a node to the graph
 *
 * @param graph   Graph object
 * @param id      Node id
 * @return  The node or NULL on error.
 */
static node_t* addNode(graph_t* graph, int id)
{
  node_t* node = NULL;

  if(graph == NULL)
    return NULL;

  node = findNode(graph, id);

  if(node != NULL)
    return node;

  // Create the node
  node = malloc(sizeof(node_t));
  node->id = id;
  node->edgecount = 0;
  node->edgespace = DFLT_EDGE_SIZE;
  node->edges = malloc(DFLT_EDGE_SIZE * sizeof(edge_t*));

  // Insert it
  graph->nodes[graph->nodecount++] = node;
  // Resize if necessary
  if(graph->nodecount >= graph->graphspace) {
    graph->graphspace *= 2;
    graph->nodes = realloc(graph->nodes, graph->graphspace);
  }

  return node;
}

/**
 * Find an edge from a node to a given end point
 *
 * @param node      Node to find the edge in
 * @param endpoint  Endpoint node id to find
 * @return  Pointer to the edge. NULL if does not exist
 */
static edge_t* findEdge(node_t* node, int endpoint)
{
  int i = 0;

  if(node == NULL)
    return NULL;

  for(i = 0 ; i < node->edgecount ; ++i) {
    if(node->edges[i]->endpoint1 == endpoint || node->edges[i]->endpoint2 == endpoint)
      return node->edges[i];
  }

  return NULL;
}

/**
 * Helper function to find an edge
 *
 * @param graph   Graph object
 * @param id1     Node id 1
 * @param id2     Node id 2
 * @return  Pointer to the edge between nodes 1 and 2. NULL otherwise.
 */
static edge_t* edgeBetween(graph_t* graph, int id1, int id2)
{
  node_t* node = NULL;

  if(graph == NULL)
    return NULL;

  node = findNode(graph, id1);
  return findEdge(node, id2);
}

/**
 * Insert an edge given a node and an edge
 *
 * @param node    Node to add edge to
 * @param edge    Edge to add
 */
static void insert_edge(node_t* node, edge_t* edge)
{
  if(node == NULL || edge == NULL)
    return;

  node->edges[node->edgecount++] = edge;
  if(node->edgecount >= node->edgespace) {
    node->edgespace *= 2;
    node->edges = realloc(node->edges, node->edgespace);
  }
}

/**
 * Remoe an edge directly given a pointer to it
 *
 * @param edge    Edge to remove
 */
static void removeEdge(edge_t* edge)
{
  node_t* n1 = NULL;
  node_t* n2 = NULL;
  int i = 0;

  if(edge == NULL)
    return;

  n1 = (node_t*)edge->p1;
  n2 = (node_t*)edge->p2;

  if(n1 == NULL || n2 == NULL)
    return;

  // This simply sets the edge to NULL in the structure
  // but does not update the edgecount (this is wasteful)
  for(i = 0 ; i < n1->edgecount ; ++i)
    if(n1->edges[i] == edge)
      n1->edges[i] = NULL;
  for(i = 0 ; i < n2->edgecount ; ++i)
    if(n2->edges[i] == edge)
      n2->edges[i] = NULL;
  free(edge);
}

/**
 * Initialize graph
 *
 * @param graph   Graph object to initialize
 */
void init_graph(graph_t* graph)
{
  if(graph == NULL)
    return;

  graph->nodecount = 0;
  graph->graphspace = GRAPH_DFLT_SIZE;
  graph->nodes = malloc(GRAPH_DFLT_SIZE * sizeof(node_t*));
}

/**
 * Destroy the graph
 *
 * @param graph   Graph object to destroy
 */
void destroy_graph(graph_t* graph)
{
  int i = 0;
  int j = 0;

  if(graph == NULL)
    return;

  for(i = 0 ; i < graph->nodecount ; ++i) {
    for(j = 0 ; j < graph->nodes[i]->edgecount ; ++j)
      removeEdge(graph->nodes[i]->edges[j]);
    free(graph->nodes[i]->edges);
    free(graph->nodes[i]);
  }
  free(graph->nodes);
}

/**
 * Update an edge to the structure. If either node
 * does not exist, the node will be created.
 * If the edge does not exist, it will also be created
 *
 * @param graph   Graph to add the node to
 * @param node1   Id of node 1
 * @param node2   Id of node 2
 * @param weight  Weight of the edge
 */
void update_edge(graph_t* graph, int node1, int node2, int weight)
{
  node_t* n1 = NULL;
  node_t* n2 = NULL;
  edge_t* edge = NULL;

  if(graph == NULL)
    return;

  edge = edgeBetween(graph, node1, node2);

  if(edge == NULL) {
    // if node exists, addNode() functions like findNode()
    n1 = addNode(graph, node1);
    n2 = addNode(graph, node2); 

    edge = malloc(sizeof(edge_t));
    edge->endpoint1 = node1;
    edge->endpoint2 = node2;
    edge->weight = weight;
    edge->p1 = n1;
    edge->p2 = n2;

    insert_edge(n1, edge);
    insert_edge(n2, edge);
  } else {
    edge->weight = weight;
  }
}

/**
 * Remove an edge between two nodes
 *
 * @param graph   Graph the nodes/edges are on
 * @param node1   Node 1 id
 * @param node2   Node 2 id
 */
void remove_edge(graph_t* graph, int node1, int node2)
{
  edge_t* edge = NULL;

  if(graph == NULL)
    return;

  edge = edgeBetween(graph, node1, node2);

  if(edge != NULL)
    edge->weight = INFINITY;
}

/**
 * Get the distance between two nodes
 *
 * @param graph   Graph the nodes are on
 * @param node1   Node1 id
 * @param node2   Node2 id
 * @return  The distance between two nodes. -1 = INFINITY.
 */
int distance_between(graph_t* graph, int node1, int node2)
{
  edge_t* edge = NULL;

  if(graph == NULL)
    return -1;

  edge = edgeBetween(graph, node1, node2);

  return (edge == NULL) ? INFINITY : edge->weight;
}

/**
 * Wrapper to findNode()
 *
 * @param graph   Input graph
 * @param node    Node id to find
 * @return  A pointer to the node or NULL if node does not exist
 */
node_t* find_node(graph_t* graph, int node)
{
  return findNode(graph, node);
}

/**
 * Find the shortest path between two nodes
 * using a simple implementation of dijkstra's algorithm:
 * http://en.wikipedia.org/wiki/Dijkstra's_algorithm
 *
 * @param graph   Graph to traverse
 * @param node1   Starting node
 * @param node2   Destination node
 * @return  The id which is the nexthop otherwise NULL if no path exists
 */
int shortest_path(graph_t* graph, int node1, int node2)
{
  node_t* u = NULL;
  node_t* v = NULL;
  node_t* target = NULL;
  edge_t* e = NULL;
  int* lut = NULL;
  int* d = NULL;
  int numnodes = 0;
  int i = 0;
  int j = 0;
  int idx = 0;
  int mindist = INFINITY;
  int minidx = -1;
  int ret = 0;

  if(graph == NULL)
    return -1;

  u = findNode(graph, node1);
  v = target = findNode(graph, node2);

  if(u == NULL || v == NULL)
    return -1;


  numnodes = u->edgecount;

  lut = malloc(numnodes * sizeof(int));
  d = malloc(numnodes * sizeof(int));
  memset(d, INFINITY, numnodes * sizeof(int));

  // lut simply maps our node id's into a range from [0, nodecount)
  for(i = 0 ; i < numnodes ; ++i) {
    e = u->edges[i];
    lut[i] = (e->p1 == u) ? ((node_t*)e->p2)->id : ((node_t*)e->p1)->id;
    d[i] = INFINITY;
  }

  u->marked = 1;

  // Now, for each neighbor, find the distance to
  // the target node
  for(i = 0 ; i < numnodes ; ++i) {
    // Unmark all nodes
    for(j = 0 ; j < graph->nodecount ; ++j) {
      if(graph->nodes[j] != u)
        graph->nodes[j]->marked = 0;
    }
    e = u->edges[i];
    if(e->weight == INFINITY) // No longer exists
      continue;
    v = (node_t*)((e->p1 == u) ? e->p2 : e->p1);
    idx = findIdx(lut, v->id, numnodes);
    d[idx] = shortest_path_helper(graph, v, target);
    d[idx] += (d[idx] != INFINITY) ? e->weight : 0;
    if((d[idx] < mindist || mindist == INFINITY) && d[idx] != INFINITY) {
      mindist = d[idx];
      minidx  = idx;
    }
  }

  ret = (minidx >= 0 && minidx < numnodes) ? lut[minidx] : -1;

  free(d);
  free(lut);

  return ret;
}

/**
 * Helper to find the shortest path
 *
 * @param graph   Graph to find the path on
 * @param src     Source node
 * @param target  The target node
 * @return  The shortest distance between src and target
 */
static int shortest_path_helper(graph_t* graph, node_t* src, node_t* target)
{
  edge_t* e = NULL;
  node_t* v = NULL;
  int i = 0;
  int mindist = INFINITY;
  int tmp = 0;

  if(src == NULL || target == NULL || src->marked)
    return INFINITY;
  if(src == target)
    return 0;
  src->marked = 1;

  for(i = 0 ; i < src->edgecount ; ++i) {
    e = src->edges[i];
    if(e->weight == INFINITY)
      continue;
    v = (node_t*)((src == e->p1) ? e->p2 : e->p1);
    tmp = shortest_path_helper(graph, v, target); 
    tmp += (tmp != INFINITY) ? e->weight : 0;
    if((tmp < mindist || mindist == INFINITY) && tmp != INFINITY)
      mindist = tmp;
  }

  src->best = mindist;

  return mindist;
}

