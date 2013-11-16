/**
 * pgps.h
 *
 * PGPS simulation
 * (specific use for 1 link with 4 flows)
 *
 * @author Dennis J. McWherter, Jr.
 */
#ifndef PGPS_H__
#define PGPS_H__

#define MAX_NUM_OF_FLOWS (4)

/**
 * Packet structure
 */
typedef struct
{
  float arrival;
  int size;
  int flow;
  float weight;
  float lastEvent;
  float lastRate;
  float remaining;
} packet_t;

/**
 * Event type
 */
typedef struct
{
  int packet_num;
  int arrival;
  int completed;
  int backlogged;
  float time;
  float delay;
} event_t;

/**
 * PGPS state
 */
typedef struct 
{
  float link_rate;
  float weight[MAX_NUM_OF_FLOWS];
  packet_t* packets;
  unsigned num_packets;
  unsigned max_packets;
} pgps_state_t;

/**
 * Results structure
 */
typedef struct
{
  float start;
  float finish;
  int size;
  int flow;
  int pgps_proc;
} time_res_t;

// Methods
int init_pgps_state(pgps_state_t*, FILE*);
void destroy_pgps_state(pgps_state_t*);
time_res_t* calc_pgps(pgps_state_t*);
time_res_t* calc_gps(pgps_state_t*);

// Static methods
static int packet_compare(const void*, const void*);
static int event_compare(const void*, const void*);
static int first_end(const void*, const void*);
static void recalc_compl(pgps_state_t*, float, float, float, event_t*, int);
static void backlog_flow(pgps_state_t*, packet_t*, event_t*, int, float, int);

#endif /** PGPS_H__ */

