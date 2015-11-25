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

/* This file handles the (bare) PARALLEL construct.  */

#include "libgomp.h"


/* Determine the number of threads to be launched for a PARALLEL construct.
   This algorithm is explicitly described in OpenMP 3.0 section 2.4.1.
   SPECIFIED is a combination of the NUM_THREADS clause and the IF clause.
   If the IF clause is false, SPECIFIED is forced to 1.  When NUM_THREADS
   is not present, SPECIFIED is 0.  */

unsigned
gomp_resolve_num_threads (unsigned specified, unsigned count)
{
  return 1;
}
/* The public OpenMP API for thread and team related inquiries.  */

int
omp_get_num_threads (void)
{
 char *env_var_str;
 int  env_var_val;
 env_var_str = getenv("OMP_NUM_THREADS");
 printf("env_var_str is %s\n",env_var_str );
 if (env_var_str != NULL) {
    sscanf(env_var_str, "%d", &env_var_val);
 }
 printf("env_var_val is %d\n", env_var_val);
 return (env_var_val<=24) ? env_var_val : 24;
}

int
omp_get_thread_num (void)
{
  return THIS_NODE_ID;
}

void
omp_set_num_threads (int n)
{
  gomp_global_icv.nthreads_var = (n > 0 ? n : 1);
}

int
omp_get_max_threads (void)
{
  return gomp_global_icv.nthreads_var;
}


void
GOMP_parallel_start (void (*fn) (void *), void *data, unsigned num_threads)
{
  /*num_threads = gomp_resolve_num_threads (num_threads, 0);*/
  gomp_team_start (fn, data, num_threads, 0, gomp_new_team (num_threads));
}

void
GOMP_parallel_end (void)
{
#ifdef DEBUG
  printf("Inside parallel end\n");
#endif //debug
}

void
GOMP_parallel (void (*fn) (void *), void *data, unsigned num_threads, unsigned int flags)
{
  num_threads = gomp_resolve_num_threads (num_threads, 0);
  gomp_team_start (fn, data, num_threads, flags, gomp_new_team (num_threads));
  fn (data);
}

bool
GOMP_single_start (void)
{
  bool ret = true;
#ifdef debug
  printf("Inside single\n");
#endif //debug
  return ret;
}

