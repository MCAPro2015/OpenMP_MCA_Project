/* Copyright (C) 2005-2015 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU Offloading and Multi Processing Library
   (libgomp).

   Libgomp is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   Libgomp is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

/* This file contains data types and function declarations that are not
   part of the official OpenACC or OpenMP user interfaces.  There are
   declarations in here that are part of the GNU Offloading and Multi
   Processing ABI, in that the compiler is required to know about them
   and use them.

   The convention is that the all caps prefix "GOMP" is used group items
   that are part of the external ABI, and the lower case prefix "gomp"
   is used group items that are completely private to the library.  */

#ifndef LIBGOMP_H
#define LIBGOMP_H 1
#define MTAPI
//#define DEBUG

#ifdef MTAPI
#include <embb/mtapi/c/mtapi.h>
#include <embb/base/c/base.h>
#include <embb/base/c/log.h>
#define MTAPI_CHECK_STATUS(status) \
    if (MTAPI_SUCCESS != status) { \
      embb_log_error("MTAPI C Example", "...error %d\n\n", status); \
      abort(); \
    }
#define THIS_DOMAIN_ID 0
#define THIS_NODE_ID 0

#define Job_ID 1
#endif //mtapi

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

 struct gomp_task_icv
{
  unsigned long nthreads_var;
  //enum gomp_schedule_type run_sched_var;
  //int run_sched_modifier;
  //int default_device_var;
  unsigned int thread_limit_var;
  bool dyn_var;
  bool nest_var;
  char bind_var;
  /* Internal ICV.  */
  //struct target_mem_desc *target_data;
};
struct gomp_task_icv gomp_global_icv;

struct gomp_task{
  mtapi_task_hndl_t task;
  int task_id;
  int group_id;
};

extern __thread struct gomp_task TLS_task;
static inline struct gomp_task *gomp_task(void)
{
  return &TLS_task;
}

struct gomp_thread
{
  /* This is the function that the thread should run upon launch.  */
  void (*fn) (void *data);
  void *data;
  //struct gomp_global_icv;

  /* This is the current team state for this thread.  The ts.team member
     is NULL only if the thread is idle.  */
  //struct gomp_team_state ts;

  /* This is the task that the thread is currently executing.  */
  //struct gomp_task *task;
  //This is the task handle for the mtapi tasks
  mtapi_task_hndl_t task_hndl[MTAPI_NODE_MAX_TASKS_DEFAULT];
  int mtapi_task_count;
  mtapi_action_hndl_t action_hndl;
  mtapi_group_hndl_t group_hndl;
  mtapi_job_hndl_t job_hndl;
  bool inside_task_group;
  /* This semaphore is used for ordered loops.  */
  //gomp_sem_t releasee;

  /* Place this thread is bound to plus one, or zero if not bound
     to any place.  */
  unsigned int place;
};
typedef struct gomp_thread gomp_thread_t;

gomp_thread_t gomp_thread_storage;
static inline gomp_thread_t *gomp_thread (void)
{
  return &gomp_thread_storage;
}






#endif /* LIBGOMP_H */
