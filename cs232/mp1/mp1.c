#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <math.h>
#include <assert.h>
#include <time.h>


typedef struct _node_t {
  struct _node_t *p[2];
} node_t;

typedef struct _graphgen_t {
  node_t *mem_range;
  size_t max_size;
  unsigned *graph;
  unsigned graph_size;
} graphgen_t;

// pick a random unused node to connect this edge to.
unsigned
select_node(graphgen_t *g, unsigned size) {
  while (1) {
	 bool failed = false;
	 unsigned x = random() % g->max_size;
	 for (unsigned int i = 0 ; i < size ; i ++) {
		if (x == g->graph[i]) {
		  failed = true;   // this node has already been used.
		  break;
		}
	 }
	 if (!failed) {
		return x;
	 }
  }
}

void
build_graph(graphgen_t *g) {
  g->graph = (unsigned *) malloc(sizeof(unsigned) * g->graph_size);
  printf("graph size = %d\n", g->graph_size);

  // create a spanning tree between the nodes
  for (unsigned int i = 0 ; i < g->graph_size ; i ++) {
	 unsigned n = select_node(g, i);
	 g->mem_range[n].p[0] = g->mem_range[n].p[0] = NULL;
	 g->graph[i] = n;

	 if (i == 0)
		continue;

	 // connect to existing graph, pick a random node.
	 while (1) {
		unsigned x = random() % i;
		node_t *parent = &g->mem_range[g->graph[x]];
		unsigned index = random() % 2;
		if (parent->p[index] == 0) {
		  parent->p[index] = &g->mem_range[n];
		  break;
		}
	 }
  }

  // fill in the extra edges
  for (unsigned int i = 0 ; i < g->graph_size ; i ++) {
	 for (int j = 0 ; j < 2 ; j ++) {
		if (g->mem_range[g->graph[i]].p[j] == 0) {
		  unsigned x = random() % g->graph_size;
		  g->mem_range[g->graph[i]].p[j] = &g->mem_range[g->graph[x]];
		}
	 }
  }
}

void
print_graph(graphgen_t *g) {
  for (unsigned int i = 0 ; i < g->max_size ; i ++) {
	 printf("%x (%3d): %x %x\n",
			  (unsigned int) &g->mem_range[i],
			  i,
			  (unsigned int) g->mem_range[i].p[0],
			  (unsigned int) g->mem_range[i].p[1]);
  }

  printf("%d:", g->graph_size);
  for (unsigned int i = 0 ; i < g->graph_size ; i ++) {
	 printf(" %d", g->graph[i]);
  }
  printf("\n");
}

// declarations of the code you have to provide
unsigned LSB_set(unsigned in);
unsigned LSB_clear(unsigned in);
unsigned bit_reverse(unsigned in);
unsigned count_nodes(node_t *node);

int
main() {
  // make sure you compiled in 32b mode
  assert(sizeof(int) == 4);
	
  // test Part 1:
  printf("The LSB_set of %x is %x and you computed %x\n", 0x02480248, 0x02480249, LSB_set(0x02480248));
  printf("The LSB_set of %x is %x and you computed %x\n", 0x02480249, 0x02480249, LSB_set(0x02480249));

  printf("The LSB_clear of %x is %x and you computed %x\n", 0x02480249, 0x02480248, LSB_clear(0x02480249));
  printf("The LSB_clear of %x is %x and you computed %x\n", 0x02480248, 0x02480248, LSB_clear(0x02480248));

  // test Part 2:
  printf("The bit_reverse of %x is %x and you computed %x\n", 0x00000001, 0x80000000, bit_reverse(0x00000001));
  printf("The bit_reverse of %x is %x and you computed %x\n", 0x00000111, 0x88800000, bit_reverse(0x00000111));
  printf("The bit_reverse of %x is %x and you computed %x\n", 0x12345678, 0x1e6a2c48, bit_reverse(0x12345678));


  // test Part 3:
  graphgen_t g;   /// allocate space for holding the graph
  g.max_size = 20480;
  g.mem_range = (node_t *) malloc(sizeof(node_t) * g.max_size);
  bzero(g.mem_range, sizeof(node_t) * g.max_size);

  // set the size of the graph to build.
  srandom( time(0) );
  g.graph_size = random() % (g.max_size/8) + g.max_size/8;
  // g.graph_size = 4    // feel free to set this as a constant for testing

  build_graph(&g);
  // print_graph(&g);

  printf("counted %d nodes\n", count_nodes(&g.mem_range[g.graph[0]]));
}


// You may make any changes you wish to the source file.  However,
// only code below this line will be considered.  DO NOT REMOVE THE LINE.
// ----------------------------------------------------------------------

unsigned 
LSB_set(unsigned in) {
  return LSB_clear(in) | 0x01;
}

unsigned 
LSB_clear(unsigned in) {
  return in & ~0x01;
}

unsigned 
bit_reverse(unsigned in) {
  int i = 0, retval = 0;
  int num_bits = 8*sizeof(unsigned);
  for(i = 0; i < num_bits-1; ++i) {
    retval |= in & 0x01;
    retval <<= 1;
    in >>= 1;
  }
  return retval;
}

unsigned
count_nodes(node_t *node) {
  node_t* orig = 0;
  unsigned pAddr = 0;

  if(node == 0)
    return 0;
  
  orig  = (node_t*)node->p[0];
  pAddr = (unsigned)orig;
  
  if(pAddr & 0x01)
    return 0;
  
  pAddr |= 0x01;
  node->p[0] = (node_t*)pAddr;
  
  return 1 + count_nodes(orig) + count_nodes(node->p[1]);
}
