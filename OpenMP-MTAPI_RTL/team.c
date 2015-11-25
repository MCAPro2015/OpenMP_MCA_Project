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

/* This file handles the maintenance of threads in response to team
   creation and termination.  */

#include "libgomp.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

__thread struct gomp_task TLS_task;

/* Create a new team data structure.  */

struct gomp_team *
gomp_new_team (unsigned nthreads)
{

}
extern void (*fn_ptr) (void*);
static void ActionFunction(
    const void* args,
    mtapi_size_t arg_size,
    void* result_buffer,
    mtapi_size_t result_buffer_size,
    const void* node_local_data,
    mtapi_size_t node_local_data_size,
    mtapi_task_context_t* task_context
  ) {
  /*struct gomp_thread *thr = gomp_thread();*/
  fn_ptr((void*)args);
}

/* Launch a team.  */
void
gomp_team_start (void (*fn) (void *), void *data, unsigned nthreads,
		 unsigned flags, struct gomp_team *team)
{
  struct gomp_thread *thr = gomp_thread ();
  mtapi_status_t status;
  //initialize the mtapi_task_count in the current thread
  /*thr->mtapi_task_count = 0;*/
 //create task group
  thr->group_hndl = mtapi_group_create(MTAPI_GROUP_ID_NONE, MTAPI_NULL,&status);
  thr->inside_task_group = true;
  /*MTAPI_CHECK_STATUS(status);*/
  //create action hndl for further reference
  thr->action_hndl = mtapi_action_create(
    Job_ID,
    (ActionFunction),
    MTAPI_NULL,
    0,
    MTAPI_DEFAULT_ACTION_ATTRIBUTES,
    &status
  );
  MTAPI_CHECK_STATUS(status);
  //get the job handle
  thr->job_hndl = mtapi_job_get(Job_ID, THIS_DOMAIN_ID, &status);
  MTAPI_CHECK_STATUS(status);
  /*printf("Inside parallel start\n");*/
}

/* Terminate the current team.  This is only to be called by the master
   thread.  We assume that we must wait for the other threads.  */

void
gomp_team_end (void)
{
  struct gomp_thread *thr = gomp_thread ();

  printf("Inside parallel end\n");
  /*mtapi_status_t status;*/
  //delete the mtapi action after executation
  /*mtapi_action_delete(thr->action_hndl, 100,&status);*/
  /*MTAPI_CHECK_STATUS(status);*/
  //delete task group
  /*mtapi_group_delete(thr->group_hndl, &status);*/
  /*MTAPI_CHECK_STATUS(status);*/
  //finalize the node
  /*mtapi_finalize(&status);*/
  /*MTAPI_CHECK_STATUS(status);*/

}


/* Constructors for this file.  */

static void __attribute__((constructor))
initialize_team (void)
{
  struct gomp_thread *thr = gomp_thread ();
  mtapi_status_t status;
  /*initialize the defaut mtapi node attributes*/
  mtapi_node_attributes_t node_attr;
  mtapi_nodeattr_init(&node_attr,&status);
  /*Set number of threads to be utilized for MTAPI runtime */
  node_attr.num_cores = omp_get_num_threads();
  MTAPI_CHECK_STATUS(status);
  /*set the mtapi node attribute to be SMP by defualt*/
  mtapi_nodeattr_set(
    &node_attr,
    MTAPI_NODE_TYPE,
    MTAPI_ATTRIBUTE_VALUE(MTAPI_NODE_TYPE_SMP),
    MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  MTAPI_CHECK_STATUS(status);
  /*initialize the master MTAPI Node for executation. */
  mtapi_info_t info;
  mtapi_initialize(
    THIS_DOMAIN_ID,
    THIS_NODE_ID,
    &node_attr,
    &info,
    &status);
  MTAPI_CHECK_STATUS(status);
  printf("MTAPI node initialized\n");

}

static void __attribute__((destructor))
team_destructor (void)
{
  mtapi_status_t status;
  /*finalize the node*/
  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);

}

struct gomp_task_icv *
gomp_new_icv (void)
{
}
