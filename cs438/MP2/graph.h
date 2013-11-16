/**
 * graph.h
 *
 * Graph
 *
 * @author Dennis J. McWherter, Jr.
 */

#ifndef GRAPH_H__
#define GRAPH_H__

/**
 * Edge structure
 */
typedef struct _edge_t
{
  int endpoint1;
  int endpoint2;
  int weight;
  void* p1;
  void* p2;
} edge_t;

/*
 * Node structure
 */
typedef struct _node_t
{
  int id;
  int edgecount;
  int edgespace;
  int marked;
  int best;
  edge_t** edges;
} node_t;

/**
 * graph structure
 */
typedef struct _graph_t
{
  int nodecount;
  int graphspace;
  node_t** nodes;
} graph_t;

// Methods
void init_graph(graph_t* graph);
void destroy_graph(graph_t* graph);
void update_edge(graph_t* graph, int node1, int node2, int weight);
void remove_edge(graph_t* graph, int node1, int node2);
int distance_between(graph_t* graph, int node1, int node2);
node_t* find_node(graph_t* graph, int node);
int shortest_path(graph_t* graph, int node1, int node2);

#endif /** GRAPH_H__ */

