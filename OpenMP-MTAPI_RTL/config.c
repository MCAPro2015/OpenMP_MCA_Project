/* This file contains system specific timer routines.  It is expected that
   a system may well want to write special versions of each of these.

   The following implementation uses the most simple POSIX routines.
   If present, POSIX 4 clocks should be used instead.  */
#define TIME_WITH_SYS_TIME

#include "libgomp.h"
#include <unistd.h>
#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif


double
omp_get_wtime (void)
{
#ifdef HAVE_CLOCK_GETTIME
  struct timespec ts;
# ifdef CLOCK_MONOTONIC
  if (clock_gettime (CLOCK_MONOTONIC, &ts) < 0)
# endif
    clock_gettime (CLOCK_REALTIME, &ts);
  return ts.tv_sec + ts.tv_nsec / 1e9;
#else
  struct timeval tv;
  gettimeofday (&tv, NULL);
  return tv.tv_sec + tv.tv_usec / 1e6;
#endif
}

double
omp_get_wtick (void)
{
#ifdef HAVE_CLOCK_GETTIME
  struct timespec ts;
# ifdef CLOCK_MONOTONIC
  if (clock_getres (CLOCK_MONOTONIC, &ts) < 0)
# endif
    clock_getres (CLOCK_REALTIME, &ts);
  return ts.tv_sec + ts.tv_nsec / 1e9;
#else
  return 1.0 / sysconf(_SC_CLK_TCK);
#endif
}

/*ialias (omp_get_wtime)*/
/*ialias (omp_get_wtick)*/

/*#include <sys/loadavg.h>*/
int
omp_get_num_procs (void)
{
  return sysconf (_SC_NPROCESSORS_ONLN);
}



