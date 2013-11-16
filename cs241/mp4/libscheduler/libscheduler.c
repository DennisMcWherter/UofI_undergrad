/*
 * Machine Problem #4
 * CS 241
 * The University of Illinois
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/* You may need to define some global variables or a struct to 
   store your job queue elements. */
typedef struct _job_t
{
  int job_number;
  int core;
  int time;
  int response;
  int priority;
  int running;
  int p_core;
  int remaining;
  int last_resp;
  int former_resp;
} job_t;

typedef struct _scheduler_t
{
  int cores;
  job_t ** core;
  scheme_t scheme;
  int totalComplete;
  int totalWaitTime;
  int totalRunningTime;
  int totalResponseTime;
} scheduler_t;

scheduler_t g_scheduler;
priqueue_t g_q;

/* First come first serve comparer */
int fcfs_comp(const void* a, const void* b)
{
  a = 0, b = 0; /* Turn off warnings */
  return -1;
}

int sjf_comp(const void* a, const void* b)
{
  job_t * t1 = (job_t*)a;
  job_t * t2 = (job_t*)b;
  int ret = t1->remaining - t2->remaining;
  return (ret == 0) ? t1->time - t2->time : ret; /* Pick a side - -1 if equal */
}

int pri_comp(const void* a, const void* b)
{
  job_t * t1 = (job_t*)a;
  job_t * t2 = (job_t*)b;
  int ret = t1->priority - t2->priority;
  return (ret == 0) ? t1->time - t2->time : ret;
}

/*
 * schedular_start_up()
 * Initalizes the scheulder.
 */
void scheduler_start_up(int cores, scheme_t scheme)
{
  int i = 0;
  g_scheduler.cores  = cores;
  g_scheduler.core   = (job_t**)malloc(cores*sizeof(job_t*));
  g_scheduler.scheme = scheme;
  g_scheduler.totalComplete = 0;
  g_scheduler.totalWaitTime = 0;
  g_scheduler.totalRunningTime = 0;
  g_scheduler.totalResponseTime = 0;
  for(i = 0; i < cores; ++i)
    g_scheduler.core[i] = 0;
  /* Eventually conditional to pick comparer */
  switch(scheme) {
    case RR: /* Round-robin */
    case FCFS: /* First come first serve */
      priqueue_init(&g_q, fcfs_comp);
    break;
    case PSJF:
    case SJF: /* Shortest job first */
      priqueue_init(&g_q, sjf_comp);
    break;
    case PPRI:
    case PRI: /* Priority ordering */
      priqueue_init(&g_q, pri_comp);
    break;
  }
}


/*
 * scheduler_new_job()
 * Called when a new job arrives.
 *
 * Returns: If the job arriving should be scheduled to run during the next
 * time cycle, return the zero-based index of the core the job should be
 * scheduled on. If another job is already running on the core specified,
 * this will preempt the currently running job.
 *
 * If multiple cores are idle, the job should be assigned to the core with the
 * lowest id.
 *
 * Otherwise, return -1.
 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
  int i = 0;
  int best = -1;
  int best_time = -1;
  int dt = 0;
  job_t * job = (job_t*)malloc(sizeof(job_t));
  job_t * curr = 0;

  job->job_number = job_number;
  job->time = time;
  job->running = running_time;
  job->priority = priority;
  job->response = -1;
  job->p_core = -1;
  job->last_resp = 0;
  job->former_resp = -1;
  job->remaining = running_time;

  switch(g_scheduler.scheme) {
    default: /* Should work for FCFS and SJF and PRI (non-preemptive) */
      for(i = 0; i < g_scheduler.cores; ++i) {
        if(g_scheduler.core[i] == 0) {
          job->response = time;
          job->p_core = i;
          g_scheduler.core[i] = job;
          return i;
        }
      }
    break;
    case PPRI: /* Priority based preemption */
      for(i = 0; i < g_scheduler.cores; ++i) {
        if((curr = g_scheduler.core[i]) == 0) {
          job->p_core = i;
          job->response = time;
          job->last_resp = time;
          g_scheduler.core[i] = job;
          return i;
        }

        if(curr->priority > job->priority) {
          if(best == -1) {
            best = i;
            best_time = curr->priority;
          } else if(best_time <= curr->priority) {
            if(best_time < curr->priority) { /* Strictly different priorities */
              best = i;
              best_time = curr->priority;
            } else if(g_scheduler.core[best]->time < curr->time) { /* Check if equal priorities which arrived first */
              best = i;
              best_time = curr->priority;
            }
          }
        }
      }

      if(best >= 0) {
        curr = g_scheduler.core[best];
        if(job->response == -1)
          job->response = time;
        job->last_resp = time;
        g_scheduler.core[best] = job;

        if(time - curr->response == 0) { /* Pre-empted */
          curr->response = -1;
          curr->last_resp = curr->former_resp;
        } else if(time - curr->last_resp == 0) {
          curr->last_resp = curr->former_resp;
        }

        priqueue_offer(&g_q, (void*)curr);
        return best;
      }
    break;
    case PSJF: /* Time preemptive */
      for(i = 0; i < g_scheduler.cores; ++i) {
        if((curr = g_scheduler.core[i]) == 0) {
          job->p_core = i;
          job->response = time;
          job->last_resp = time;
          g_scheduler.core[i] = job;
          return i;
        }

        dt = time - curr->last_resp;
        if((curr->remaining - dt) > job->remaining) {
          if(best == -1) {
            best = i;
            best_time = curr->remaining - dt;
          } else if(best_time < (curr->remaining - dt)) {
            best = i;
            best_time = curr->remaining - dt;
          }
        }
      }

      if(best >= 0) { /* Weak-sauce.. Pre-empted... */
        curr = g_scheduler.core[best];

        if(job->response == -1)
          job->response = time;
        job->last_resp = time;
        job->p_core = best;
        g_scheduler.core[best] = job;

        curr->remaining = best_time;

        if(time - curr->response == 0) { /* Scheduled last time, but pre-empted now */
          curr->response = -1;
          curr->last_resp = curr->former_resp;
        } else if(time - curr->last_resp == 0) {
          curr->last_resp = curr->former_resp;
        }

        priqueue_offer(&g_q, (void*)curr);
        return best;
      }
    break;
  }

  priqueue_offer(&g_q, (void*)job);

	return -1;
}


/*
 * scheduler_job_finished()
 * Called when a job has completed execution.
 *
 * Returns: If any job should be scheduled to run on the core free'd up by the
 * finished job, return the job_number of the job that should be scheduled to
 * run on core core_id.
 *
 * Otherwise, if the core should remain idle, return -1.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
  job_t * job = 0;
  job_number = 0; /* Stop compiler warnings - I don't use this */

  /* Update values for averages */
  job = g_scheduler.core[core_id];
  g_scheduler.totalComplete++;
  g_scheduler.totalWaitTime += time-job->time-job->running;
  g_scheduler.totalRunningTime += time-job->time;
  g_scheduler.totalResponseTime += job->response-job->time;

  free(job);
  g_scheduler.core[core_id] = 0;
  job = (job_t*)priqueue_poll(&g_q);
  if(job != 0) {
    if(job->response == -1){ /* Means first time we're responding */
      job->former_resp = -1;
      job->response = time;
    } else
      job->former_resp = job->last_resp;
    job->last_resp = time;
    g_scheduler.core[core_id] = job;
    return job->job_number;
  }
  return -1;
}


/*
 * scheduler_quantum_expired()
 * When the scheme is set to RR, called when the quantum timer has expired
 * on a core.
 *
 * Returns: If any job should be scheduled to run on the core free'd up by
 * the quantum expiration, return the job_number of the job that should be
 * scheduled to run on core core_id.
 *
 * Otherwise, if the core should remain idle, return -1.
 */
int scheduler_quantum_expired(int core_id, int time)
{
  job_t * job = 0;
  job_t * curr = g_scheduler.core[core_id];

  job = (job_t*)priqueue_poll(&g_q);

  if(job != 0) {
    if(job->response == -1)
      job->response = time;
    job->last_resp = time;
    g_scheduler.core[core_id] = job;
    if(curr != 0)
      priqueue_offer(&g_q, (void*)curr);
    return job->job_number;
  } else if(curr != 0) {
    return curr->job_number;
  }

	return -1;
}


/*
 * scheduler_average_waiting_time()
 * Returns the average waiting time of all jobs scheduled by your scheduler.
 */
float scheduler_average_waiting_time()
{
	return (float)g_scheduler.totalWaitTime/g_scheduler.totalComplete;
}


/*
 * scheduler_average_turnaround_time()
 * Returns the average turnaround time of all jobs scheduled by
 * your scheduler.
 */
float scheduler_average_turnaround_time()
{
	return (float)g_scheduler.totalRunningTime/g_scheduler.totalComplete;
}


/*
 * scheduler_average_response_time()
 * Return the average response time of all jobs scheduled by your scheduler.
 */
float scheduler_average_response_time()
{
	return (float)g_scheduler.totalResponseTime/g_scheduler.totalComplete;
}


/*
 * scheduler_clean_up()
 * Free any memory associated with your scheduler.
 */
void scheduler_clean_up()
{
  int i = 0;
  job_t * tmp = 0;
  for(i = 0; i < g_scheduler.cores; ++i) /* If there is anything running still */
    if(g_scheduler.core[i] != 0)
      free(g_scheduler.core[i]);
  for(i = 0; i < priqueue_size(&g_q); ++i) /* Just in case we have anything enqueued */
    if((tmp = (job_t*)priqueue_poll(&g_q)) != 0)
      free(tmp);
  free(g_scheduler.core);
  priqueue_destroy(&g_q);
}


/*
 * scheduler_show_queue()
 * This function may print out any debugging information you choose. This
 * function will be called by the simulator after every call the simulator
 * makes to your scheduler.
 *
 * This function is not required and will not be graded. You may leave it
 * blank if you do not find it useful.
 */
void scheduler_show_queue()
{
  int i = 0;
  job_t * job = 0;
  for(i = 0; i < g_scheduler.cores; ++i) {
    job = g_scheduler.core[i];
    if(g_scheduler.core[i] != 0)
      printf("%d(%d) ", job->job_number, i);
  }
  for(i = 0; i < priqueue_size(&g_q); ++i) {
    job = (job_t*)priqueue_at(&g_q, i);
    if(job != 0)
      printf("%d(%d) ", job->job_number, job->p_core); /* Priority should be the number core its on */
  }
}


