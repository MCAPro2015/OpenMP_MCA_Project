/*******************************************************
 * C file translated from WHIRL Fri Nov  9 11:36:37 2012
 *******************************************************/

/* Include file-level type and variable decls */
#include "Dijkstra_kernel.w2c.h"


__inline _INT32 pthread_equal(
  _UINT64 __thread1,
  _UINT64 __thread2)
{
  
  
  return __thread1 == __thread2;
} /* pthread_equal */


extern void dowork()
{
  
  register _INT32 _w2c___ompv_ok_to_fork;
  register _UINT64 _w2c_reg3;
  register _INT32 _w2c___comma;
  register _INT32 _w2c___comma0;
  _INT32 __ompv_gtid_s1;
  
  /*Begin_of_nested_PU(s)*/
  
  _w2c___ompv_ok_to_fork = 1;
  if(_w2c___ompv_ok_to_fork)
  {
    _w2c___ompv_ok_to_fork = __ompc_can_fork();
  }
  if(_w2c___ompv_ok_to_fork)
  {
    __ompc_fork(0, &__omprg_dowork_1, _w2c_reg3);
  }
  else
  {
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    __ompc_serialized_parallel();
    _w2c___comma = omp_get_thread_num();
    me = _w2c___comma;
    _w2c___comma0 = omp_get_num_threads();
    nth = _w2c___comma0;
    if((nv % nth) != 0)
    {
      printf("nv must be divisible by nth\n");
      exit(1);
    }
    chunk = nv / nth;
    startv = me * chunk;
    endv = (chunk + startv) + -1;
    step = 0;
    while(nv > step)
    {
      _514 :;
      md = largeint;
      mv = 0;
      findmymin(startv, endv, &mymd, &mymv);
      __ompc_critical(__ompv_gtid_s1, (_UINT64) & __mplock_0);
      if((_UINT32) md > mymd)
      {
        md = (_INT32) mymd;
        mv = mymv;
      }
      __ompc_end_critical(__ompv_gtid_s1, (_UINT64) & __mplock_0);
      * (notdone + (_UINT64)((_UINT64) mv)) = 0;
      updatemind(startv, endv);
      step = step + 1;
      _258 :;
    }
    goto _770;
    _770 :;
    __ompc_end_serialized_parallel();
  }
  return;
} /* dowork */


static void __omprg_dowork_1(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT64 __ompv_slink_a;
{
  
  register _INT32 _w2c___comma;
  register _INT32 _w2c_mp_is_master;
  register _INT32 _w2c___comma0;
  register _INT32 _w2c_mp_is_master0;
  register _INT32 _w2c_mp_is_master1;
  _UINT64 _temp___slink_sym0;
  _INT32 __ompv_temp_gtid;
  
  /*Begin_of_nested_PU(s)*/
  
  _temp___slink_sym0 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  _w2c___comma = omp_get_thread_num();
  me = _w2c___comma;
  _w2c_mp_is_master = __ompc_master(__ompv_temp_gtid);
  if(_w2c_mp_is_master == 1)
  {
    _w2c___comma0 = omp_get_num_threads();
    nth = _w2c___comma0;
    if((nv % nth) != 0)
    {
      printf("nv must be divisible by nth\n");
      exit(1);
    }
    chunk = nv / nth;
  }
  __ompc_end_master(__ompv_temp_gtid);
  startv = me * chunk;
  endv = (chunk + startv) + -1;
  step = 0;
  while(nv > step)
  {
    _259 :;
    _w2c_mp_is_master0 = __ompc_master(__ompv_temp_gtid);
    if(_w2c_mp_is_master0 == 1)
    {
      md = largeint;
      mv = 0;
    }
    __ompc_end_master(__ompv_temp_gtid);
    findmymin(startv, endv, &mymd, &mymv);
    __ompc_critical(__ompv_temp_gtid, (_UINT64) & __mplock_0);
    if((_UINT32) md > mymd)
    {
      md = (_INT32) mymd;
      mv = mymv;
    }
    __ompc_end_critical(__ompv_temp_gtid, (_UINT64) & __mplock_0);
    __ompc_ebarrier();
    _w2c_mp_is_master1 = __ompc_master(__ompv_temp_gtid);
    if(_w2c_mp_is_master1 == 1)
    {
      * (notdone + (_UINT64)((_UINT64) mv)) = 0;
    }
    __ompc_end_master(__ompv_temp_gtid);
    updatemind(startv, endv);
    __ompc_ebarrier();
    step = step + 1;
    _515 :;
  }
  _771 :;
  __ompc_task_exit();
  return;
} /* __omprg_dowork_1 */

