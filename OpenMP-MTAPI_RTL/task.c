/* This file handles the maintainence of tasks in response to task
   creation and termination.  */

#include "libgomp.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef MTAPI
void (*fn_ptr) (void *);
/*mtapi_job_hndl_t job_hndl;*/
//count of how many tasks creaeted
int count_start=0;
int count_wait=0;
//define the action function for task executation
#endif //MTAPI

void
GOMP_taskgroup_start (void)
{
  struct gomp_thread *thr = gomp_thread();
  mtapi_status_t status;
  thr->group_hndl = mtapi_group_create(MTAPI_GROUP_ID_NONE, MTAPI_NULL, &status);
  MTAPI_CHECK_STATUS(status);
  thr->inside_task_group = true;
}

void
GOMP_taskgroup_end (void)
{
  struct gomp_thread *thr = gomp_thread();
  mtapi_status_t status;
  mtapi_group_wait_all(thr->group_hndl,MTAPI_INFINITE,&status);
  MTAPI_CHECK_STATUS(status);
  thr->inside_task_group = false;
}

    /* Called when encountering an explicit task directive.  If IF_CLAUSE is
   false, then we must not delay in executing the task.  If UNTIED is true,
   then the task may be executed by any member of the team.  */

void
GOMP_task (void (*fn) (void *), void *data, void (*cpyfn) (void *, void *),
	   long arg_size, long arg_align, bool if_clause, unsigned flags,
	   void **depend)
{
  struct gomp_thread *thr = gomp_thread ();
  /*assign the function pointer to the TLS storage*/
  fn_ptr = fn;
  /*GOMP_taskgroup_start();*/
  mtapi_status_t status;
  mtapi_task_hndl_t task;
  /* create action */
  /*printf("MTAPI task create, count %d\n",count);*/
  /*if(thr->inside_task_group){*/
    task =
  mtapi_task_start(
      MTAPI_TASK_ID_NONE,
      thr->job_hndl,
      data,
      sizeof(data),
      NULL,
      sizeof(NULL),
      MTAPI_DEFAULT_TASK_ATTRIBUTES,
      thr->group_hndl,
      &status
  );
  /*}*/
  /*else{//not inside a task group*/
    /*task =*/
  /*mtapi_task_start(*/
      /*MTAPI_TASK_ID_NONE,*/
      /*thr->job_hndl,*/
      /*data,*/
      /*sizeof(data),*/
      /*NULL,*/
      /*sizeof(NULL),*/
      /*MTAPI_DEFAULT_TASK_ATTRIBUTES,*/
      /*MTAPI_GROUP_NONE,*/
      /*&status*/
  /*);*/
  /*}*/
 #ifdef DEBUG
  printf("MTAPI task started, count %d\n",count_start++);
#endif //DEBUG
  /*printf("mtapi_task_count is %d\n", thr->mtapi_task_count);*/
  /*thr->task_hndl[thr->mtapi_task_count++] = task;*/
  mtapi_task_wait(task,MTAPI_INFINITE, &status);
//in the current implementation we need the task wait to work around the dependancy
//analysis of the tasks.
  /*mtapi_task_wait(task,MTAPI_INFINITE, &status);*/
  //MTAPI_CHECK_STATUS(status);
}

/* Called when encountering a taskwait directive.  */

void
GOMP_taskwait (void)
{
  struct gomp_thread *thr = gomp_thread ();
  #ifdef MTAPI
#ifdef DEBUG
  printf("MTAPI task wait, count %d\n",count_wait++);
#endif  //debug
  /*int i;*/
  mtapi_status_t status;
  /*GOMP_taskgroup_end();*/
  /*Need to create an array of tasks, stored as TLS, need to send */
    /*all of the tasks to the task wait funciton for explicit waiting.  */
  /*for(i=0;i<thr->mtapi_task_count;i++){*/
     /*mtapi_task_wait(thr->task_hndl[i],MTAPI_INFINITE, &status);*/
  /*}*/
  /*mtapi_group_wait_any(thr->group_hndl,MTAPI_NULL,MTAPI_INFINITE,&status);*/
  /*mtapi_group_wait_all(thr->group_hndl,MTAPI_INFINITE,&status);*/
  MTAPI_CHECK_STATUS(status);
  #endif //MTAPI

}

void
GOMP_barrier (void)
{
  struct gomp_thread *thr = gomp_thread ();
  mtapi_status_t status;
/*The mtapi_group_wait_all will wait for all the tasks in the same group hndl, thus performs*/
  /*the similar functionality of the barrier in this runtime library*/
  /*mtapi_group_wait_all(thr->group_hndl,MTAPI_INFINITE,&status);*/
  MTAPI_CHECK_STATUS(status);
}

bool
GOMP_barrier_cancel (void)
{
  return false;
  /* The compiler transforms to barrier_cancel when it sees that the
     barrier is within a construct that can cancel.  Thus we should
     never have an orphaned cancellable barrier.  */
}

void
GOMP_taskyield (void)
{
  /* Nothing at the moment.  */
}

