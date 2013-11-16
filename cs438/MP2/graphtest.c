/**
 *  Graph test
 */

#include "graph.h"

int main()
{
  graph_t graph;
  init_graph(&graph);

  update_edge(&graph, 10, 7, 1);
  update_edge(&graph, 10, 5, 9);
  update_edge(&graph, 5, 7, 10);
  update_edge(&graph, 0, 10, 2);

  remove_edge(&graph, 0, 10);

  shortest_path(&graph, 10, 5);

  remove_edge(&graph, 10, 5);

  shortest_path(&graph, 10, 5);

  destroy_graph(&graph);
  
  // More advanced graph
  init_graph(&graph);

  update_edge(&graph, 1, 6, 1);
  update_edge(&graph, 6, 4, 5);
  update_edge(&graph, 4, 5, 7);
  update_edge(&graph, 5, 3, 3);
  update_edge(&graph, 3, 2, 8);
  update_edge(&graph, 6, 2, 20);

  shortest_path(&graph, 1, 2);

  update_edge(&graph, 6, 2, 25);

  shortest_path(&graph, 1, 2);

  remove_edge(&graph, 6, 2);
  remove_edge(&graph, 3, 2);

  // No path should exist
  shortest_path(&graph, 1, 2);

  destroy_graph(&graph);

  return 0;
}

