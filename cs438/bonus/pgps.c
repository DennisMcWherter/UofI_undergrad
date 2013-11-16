/**
 * pgps.c
 *
 * PGPS Simulation
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pgps.h"

int main(int argc, char** argv)
{
  int i = 0;
  int num_packets = 0;
  FILE* fp = NULL;
  pgps_state_t state;
  time_res_t* pgps = NULL;

  if(argc < 3) {
    fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
    return -1;
  }

  if((fp = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Could not read file: %s\n", argv[1]);
    return -1;
  }

  if(init_pgps_state(&state, fp) < 0) {
    fprintf(stderr, "Could not parse input file?\n");
    return -1;
  }

  fclose(fp);
  fp = NULL;

  num_packets = state.num_packets;
  pgps = calc_pgps(&state);

  destroy_pgps_state(&state);

  if((fp = fopen(argv[2], "w")) == NULL) {
    fprintf(stderr, "Could not open file for writing: %s\n", argv[2]);
    free(pgps);
    return -1;
  }

  if(pgps != NULL) {
    for(i = 0 ; i < num_packets ; ++i) {
      // Print to terminal and write out to file
      printf("%.3f %d \n", pgps[i].start, pgps[i].flow);
      fprintf(fp, "%.3f %d \n", pgps[i].start, pgps[i].flow);
    }
  }

  free(pgps);
  fclose(fp);

  return 0;
}

int init_pgps_state(pgps_state_t* state, FILE* fp)
{
  packet_t* packet = NULL;
  if(fp == NULL || state == NULL)
    return -1; // Error

  // Initialize the state
  state->link_rate   = 0;
  state->num_packets = 0;
  state->max_packets = 2; // Arbitrary start point
  state->packets = malloc(sizeof(packet_t) * state->max_packets);

  if(fscanf(fp, "%f %f %f %f\n", &state->weight[0], &state->weight[1], &state->weight[2], &state->weight[3]) != 4)
    return -1;

  if(fscanf(fp, "%f\n", &state->link_rate) != 1)
    return -1;

  while(!feof(fp)) {
    if(state->num_packets >= state->max_packets) {
      state->max_packets *= 2;
      state->packets = realloc(state->packets, state->max_packets * sizeof(packet_t));
    }
    packet = &state->packets[state->num_packets];
    if(fscanf(fp, "%f %d %d\n", &packet->arrival, &packet->size, &packet->flow) != 3)
      return -1;
    packet->size *= 8; // Bytes to bits
    packet->weight = state->weight[packet->flow - 1];
    packet->lastEvent = -1.f;
    packet->lastRate  = 0.f;
    packet->remaining = (float)packet->size;
    state->num_packets++;
  }

  // Sort the packets by arrival time
  qsort(state->packets, state->num_packets, sizeof(packet_t), packet_compare);

  return 1;
}

void destroy_pgps_state(pgps_state_t* state)
{
  if(state == NULL)
    return;

  free(state->packets);
}

time_res_t* calc_pgps(pgps_state_t* state)
{
  int i = 0;
  int found = 0;
  float recent_end_time = INFINITY;
  float rate = 0.f;
  unsigned num_res = 0;
  unsigned remaining = 0;
  time_res_t* gps = NULL;
  time_res_t* res = NULL;

  if((gps = calc_gps(state)) == NULL)
    return NULL;

  remaining = state->num_packets;
  rate      = state->link_rate;

  res = malloc(state->num_packets * sizeof(time_res_t));

  // Sort by which ends first
  qsort(gps, state->num_packets, sizeof(time_res_t), first_end);

  // Find the earliest start time
  for(i = 0 ; i < state->num_packets ; ++i) {
    if(gps[i].start < recent_end_time)
      recent_end_time = gps[i].start;
    gps[i].pgps_proc = 0; // Just double check
  }

  while(remaining > 0) {
    for(i = 0 ; i < state->num_packets ; ++i) {
      // Idea: Pick the next packet which is supposed to finish in
      // GPS iff that packet has already arrived
      if(gps[i].pgps_proc || gps[i].start > recent_end_time)
        continue;

      res[num_res].flow   = gps[i].flow;
      res[num_res].size   = gps[i].size;
      res[num_res].start  = recent_end_time;
      res[num_res].finish = recent_end_time + (res[num_res].size / rate);
      gps[i].pgps_proc = 1;

      recent_end_time = res[num_res].finish;
      found = 1;
      num_res++;
      break;
    }
    if(!found) { // There is a gap of time where we are completely unutilized
      recent_end_time = INFINITY;
      for(i = 0 ; i < state->num_packets ; ++i) {
        if(gps[i].pgps_proc == 0 && gps[i].start < recent_end_time)
          recent_end_time = gps[i].start;
      }
    } else {
      remaining--;
      found = 0;
    }
  }

  free(gps);

  return res;
}

time_res_t* calc_gps(pgps_state_t* state)
{
  int i = 0;
  float usage = 0.f;
  unsigned num_res = 0;
  unsigned num_events = 0;
  unsigned curr_evts  = 0;
  unsigned events_processed = 0;
  time_res_t* res = NULL;
  packet_t* packet = NULL;
  event_t* event = NULL;
  event_t* nextEvent = NULL;

  if(state == NULL)
    return NULL;

  // There will be exactly 2 * num_packets events (one arrival and one leave)
  // But we will use a clever trick to turn arrivals into completions after the
  // arrival has been processed
  num_events = state->num_packets;
  curr_evts  = 0;
  events_processed = 0;

  res = malloc(state->num_packets * sizeof(time_res_t));
  nextEvent = malloc(num_events * sizeof(event_t));
  memset(nextEvent, 0, num_events * sizeof(event_t));

  for(i = 0 ; i < num_events ; ++i) {
    nextEvent[i].packet_num = i;
    nextEvent[i].time = state->packets[i].arrival; // Arrivals are first events
    nextEvent[i].arrival = 1; // These are all the arrivals
    nextEvent[i].completed = 0;
    nextEvent[i].backlogged = 0;
    nextEvent[i].delay = 0.f;
  }

  // For each flow, Sum of ((w_i) / (Sum of j in Backlogged w_j)) is portion link
  // Two events which change flow-rate: packet ends, packet arrives 
  while(num_res < state->num_packets) {
    event = &nextEvent[0]; // Always sort so this is at the top

    packet = &state->packets[event->packet_num];

    if(event->arrival == 1) { // Arrival
      usage += packet->weight;
      packet->lastEvent = event->time;
      packet->lastRate  = 0.f;
      event->arrival = 0; // Arrival processed, turn it into completios

      // Backlogged flows
      backlog_flow(state, packet, nextEvent, num_events, 0, 1);
    } else {
      usage -= packet->weight;
      event->completed = 1;

      // Store the information
      res[num_res].start  = (event->delay > packet->arrival) ? event->delay : packet->arrival;
      res[num_res].size   = packet->size;
      res[num_res].flow   = packet->flow;
      res[num_res].finish = event->time;
      res[num_res].pgps_proc = 0;

      // Remove the fflow from being backlogged
      backlog_flow(state, packet, nextEvent, num_events, event->time, 0);

      num_res++;
    }

    // Recalculate the completion times
    recalc_compl(state, (event->delay > event->time) ? event->delay : event->time, usage, state->link_rate, nextEvent, num_events);
    qsort(nextEvent, num_events, sizeof(event_t), event_compare); // Re-sort events
  }

  free(nextEvent);

  return res;
}

void recalc_compl(pgps_state_t* state, float time, float usage, float link_rate, event_t* events, int num_evts)
{
  int i = 0;
  int packet_num = 0;
  float weight = 0.f;
  float rate = 0.f;
  float elapsed = 0.f;
  packet_t* packet = NULL;

  if(state == NULL || events == NULL || usage == 0.f)
    return;

  for(i = 0 ; i < num_evts ; ++i) {
    if(events[i].completed || events[i].backlogged)
      continue; // Skip already processed or backlogged packets 

    packet_num = events[i].packet_num;
    packet = &state->packets[packet_num];

    if(packet->lastEvent == -1.f) 
      continue; // This packet has not yet arrived

    weight  = packet->weight;
    rate    = (weight / usage) * link_rate;
    elapsed = time - packet->lastEvent;
    packet->remaining -= (packet->lastRate * elapsed);
    packet->lastEvent = time;
    packet->lastRate  = rate;

    events[i].time = time + (packet->remaining / rate);
  }
}

void backlog_flow(pgps_state_t* state, packet_t* packet, event_t* events, int num_evts, float time, int val)
{
  int i = 0;
  packet_t* tmp = NULL;
  if(state == NULL || packet == NULL || events == NULL)
    return;
  for(i = 0 ; i < num_evts ; ++i) {
    tmp = &state->packets[events[i].packet_num];
    if(tmp != packet && tmp->flow == packet->flow) {
      if(events[i].time < time)
        events[i].delay = time;
      events[i].backlogged = val;
    }
  }
}

/** Static */
int packet_compare(const void* a, const void* b)
{
  packet_t* p1 = (packet_t*)a;
  packet_t* p2 = (packet_t*)b;
  float diff = 0;
  if(a == NULL || b == NULL)
    return 0;
  diff = p1->arrival - p2->arrival;
  // Try by arrival, otherwise by flow id if they are the same time
  return (diff == 0) ? (p1->flow - p2->flow) : p1->arrival > p2->arrival;
}

int event_compare(const void* a, const void* b)
{
  event_t* f1 = (event_t*)a;
  event_t* f2 = (event_t*)b;
  float diff = 0;
  if(a == NULL || b == NULL)
    return 0;
  diff = f1->time - f2->time;
  if(f1->completed || f1->backlogged)
    return 1;
  else if(f2->completed || f2->backlogged)
    return -1;
  return (diff == 0) ? (f1->packet_num - f2->packet_num) : f1->time > f2->time;
}

int first_end(const void* a, const void* b)
{
  time_res_t* r1 = (time_res_t*)a;
  time_res_t* r2 = (time_res_t*)b;
  if(a == NULL || b == NULL)
    return 0;
  return r1->finish - r2->finish;
}

