/*******************************************************
 * C file translated from WHIRL Thu Dec  6 12:49:35 2012
 *******************************************************/

/* Include file-level type and variable decls */
#include "omp_mm.w2c.h"


__inline _INT32 pthread_equal(
  _UINT64 __thread1,
  _UINT64 __thread2)
{
  
  
  return __thread1 == __thread2;
} /* pthread_equal */


extern _INT32 main(
  _INT32 argc,
  _INT8 ** argv)
{
  
  register _INT32 _w2c___comma;
  register _UINT64 _w2c___comma0;
  register _UINT64 _w2c___comma1;
  register _UINT64 _w2c___comma2;
  register _UINT64 _w2c___comma3;
  register _UINT64 _w2c___comma4;
  register _UINT64 _w2c___comma5;
  register _INT32 _w2c___comma6;
  register _INT32 _w2c___ompv_ok_to_fork;
  register _UINT64 _w2c_reg3;
  register _INT32 _w2c___comma7;
  struct timeval start;
  struct timeval end;
  _IEEE64 t1;
  _IEEE64 t2;
  _INT32 __ompv_gtid_s1;
  
  /*Begin_of_nested_PU(s)*/
  
  if(argc != 2)
  {
    printf("usage: %s <#dimensions> \n", *argv);
    exit(-1);
  }
  _w2c___comma = atoi(*(argv + 1LL));
  N = _w2c___comma;
  _w2c___comma0 = (_UINT64)(_UINT64) malloc((_UINT64)((_UINT64) N) * 8ULL);
  a = (_IEEE64 **) _w2c___comma0;
  _w2c___comma1 = (_UINT64)(_UINT64) malloc((_UINT64)((_UINT64) N) * 8ULL);
  b = (_IEEE64 **) _w2c___comma1;
  _w2c___comma2 = (_UINT64)(_UINT64) malloc((_UINT64)((_UINT64) N) * 8ULL);
  c = (_IEEE64 **) _w2c___comma2;
  i = 0;
  while(N > i)
  {
    _514 :;
    _w2c___comma3 = (_UINT64)(_UINT64) malloc((_UINT64)((_UINT64) N) * 8ULL);
    * (a + (_UINT64)((_UINT64) i)) = (_IEEE64 *) _w2c___comma3;
    _w2c___comma4 = (_UINT64)(_UINT64) malloc((_UINT64)((_UINT64) N) * 8ULL);
    * (b + (_UINT64)((_UINT64) i)) = (_IEEE64 *) _w2c___comma4;
    _w2c___comma5 = (_UINT64)(_UINT64) malloc((_UINT64)((_UINT64) N) * 8ULL);
    * (c + (_UINT64)((_UINT64) i)) = (_IEEE64 *) _w2c___comma5;
    i = i + 1;
    _258 :;
  }
  goto _770;
  _770 :;
  _w2c___comma6 = atoi(*(argv + 1LL));
  printf("The dimension value is %d: \n", _w2c___comma6);
  _w2c___ompv_ok_to_fork = 1;
  if(_w2c___ompv_ok_to_fork)
  {
    _w2c___ompv_ok_to_fork = __ompc_can_fork();
  }
  if(_w2c___ompv_ok_to_fork)
  {
    __ompc_fork(0, &__omprg_main_1, _w2c_reg3);
  }
  else
  {
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    __ompc_serialized_parallel();
    _w2c___comma7 = omp_get_num_threads();
    threads = _w2c___comma7;
    printf("num of threads is %d\n", threads);
    __ompc_end_serialized_parallel();
  }
  init();
  gettimeofday(&start, (struct timezone *) 0ULL);
  t1 = (_IEEE64)((start).tv_sec) + ((_IEEE64)((start).tv_usec) / 1.0e+06);
  mmm();
  gettimeofday(&end, (struct timezone *) 0ULL);
  t2 = (_IEEE64)((end).tv_sec) + ((_IEEE64)((end).tv_usec) / 1.0e+06);
  verify();
  printf("the parallel wall time is %lf sec\n", t2 - t1);
  return 0;
} /* main */


static void __omprg_main_1(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT64 __ompv_slink_a;
{
  
  register _INT32 _w2c_mp_is_master;
  register _INT32 _w2c___comma;
  _UINT64 _temp___slink_sym0;
  _INT32 __ompv_temp_gtid;
  
  /*Begin_of_nested_PU(s)*/
  
  _temp___slink_sym0 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  _w2c_mp_is_master = __ompc_master(__ompv_temp_gtid);
  if(_w2c_mp_is_master == 1)
  {
    _w2c___comma = omp_get_num_threads();
    threads = _w2c___comma;
    printf("num of threads is %d\n", threads);
  }
  __ompc_end_master(__ompv_temp_gtid);
  __ompc_task_exit();
  return;
} /* __omprg_main_1 */


extern void init()
{
  
  
  i = 0;
  while(N > i)
  {
    _514 :;
    j = 0;
    while(N > j)
    {
      _1026 :;
      * (*(a + (_UINT64)((_UINT64) i)) + (_UINT64)((_UINT64) j)) = (_IEEE64)(i);
      * (*(b + (_UINT64)((_UINT64) i)) + (_UINT64)((_UINT64) j)) = (_IEEE64)(i);
      * (*(c + (_UINT64)((_UINT64) i)) + (_UINT64)((_UINT64) j)) = 0.0;
      j = j + 1;
      _770 :;
    }
    goto _1282;
    _1282 :;
    i = i + 1;
    _258 :;
  }
  goto _1538;
  _1538 :;
  return;
} /* init */


extern void mmm()
{
  
  register _INT32 _w2c___ompv_ok_to_fork;
  register _UINT64 _w2c_reg3;
  _INT32 __localized_common_i;
  _INT32 __localized_common_k;
  _INT32 __localized_common_j;
  _INT32 _temp__mp_xpragma7;
  _INT32 __ompv_gtid_s1;
  
  /*Begin_of_nested_PU(s)*/
  
  _temp__mp_xpragma7 = threads;
  _w2c___ompv_ok_to_fork = 1;
  if(_w2c___ompv_ok_to_fork)
  {
    _w2c___ompv_ok_to_fork = __ompc_can_fork();
  }
  if(_w2c___ompv_ok_to_fork)
  {
    __ompc_fork(_temp__mp_xpragma7, &__omprg_mmm_1, _w2c_reg3);
  }
  else
  {
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    __ompc_serialized_parallel();
    for(__localized_common_i = 0; __localized_common_i < N; __localized_common_i = __localized_common_i + 1)
    {
      __localized_common_k = 0;
      while(__localized_common_k < N)
      {
        _514 :;
        __localized_common_j = 0;
        while(__localized_common_j < N)
        {
          _1026 :;
          * (*(c + (_UINT64)((_UINT64) __localized_common_i)) + (_UINT64)((_UINT64) __localized_common_j)) = *(*(c + (_UINT64)((_UINT64) __localized_common_i)) + (_UINT64)((_UINT64) __localized_common_j)) + (*(*(a + (_UINT64)((_UINT64) __localized_common_i)) + (_UINT64)((_UINT64) __localized_common_k)) ** (*(b + (_UINT64)((_UINT64) __localized_common_k)) + (_UINT64)((_UINT64) __localized_common_j)));
          __localized_common_j = __localized_common_j + 1;
          _770 :;
        }
        goto _1282;
        _1282 :;
        __localized_common_k = __localized_common_k + 1;
        _258 :;
      }
      goto _1538;
      _1538 :;
    }
    __ompc_end_serialized_parallel();
  }
  return;
} /* mmm */


static void __omprg_mmm_1(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT64 __ompv_slink_a;
{
  
  register _INT32 _w2c_temp_limit;
  register _INT32 _w2c_temp_base;
  _UINT64 _temp___slink_sym8;
  _INT32 __ompv_temp_gtid;
  _INT32 __mplocal___localized_common_j;
  _INT32 __mplocal___localized_common_k;
  _INT32 __mplocal___localized_common_i;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _INT32 __ompv_temp_last_iter;
  
  /*Begin_of_nested_PU(s)*/
  
  _temp___slink_sym8 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  _w2c_temp_limit = N + -1;
  _w2c_temp_base = 0;
  __ompv_temp_do_upper0 = _w2c_temp_limit;
  __ompv_temp_do_lower0 = 0;
  __ompv_temp_last_iter = 0;
  __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower0, &__ompv_temp_do_upper0, &__ompv_temp_do_stride0, 1, 1);
  if(__ompv_temp_do_upper0 > _w2c_temp_limit)
  {
    __ompv_temp_do_upper0 = _w2c_temp_limit;
  }
  for(__mplocal___localized_common_i = __ompv_temp_do_lower0; __mplocal___localized_common_i <= __ompv_temp_do_upper0; __mplocal___localized_common_i = __mplocal___localized_common_i + 1)
  {
    __mplocal___localized_common_k = 0;
    while(__mplocal___localized_common_k < N)
    {
      _259 :;
      __mplocal___localized_common_j = 0;
      while(__mplocal___localized_common_j < N)
      {
        _515 :;
        * (*(c + (_UINT64)((_UINT64) __mplocal___localized_common_i)) + (_UINT64)((_UINT64) __mplocal___localized_common_j)) = *(*(c + (_UINT64)((_UINT64) __mplocal___localized_common_i)) + (_UINT64)((_UINT64) __mplocal___localized_common_j)) + (*(*(a + (_UINT64)((_UINT64) __mplocal___localized_common_i)) + (_UINT64)((_UINT64) __mplocal___localized_common_k)) ** (*(b + (_UINT64)((_UINT64) __mplocal___localized_common_k)) + (_UINT64)((_UINT64) __mplocal___localized_common_j)));
        __mplocal___localized_common_j = __mplocal___localized_common_j + 1;
        _771 :;
      }
      _1027 :;
      __mplocal___localized_common_k = __mplocal___localized_common_k + 1;
      _1283 :;
    }
    _1539 :;
  }
  __ompc_barrier();
  __ompc_task_exit();
  return;
} /* __omprg_mmm_1 */


extern void verify()
{
  
  
  i = 0;
  while(N > i)
  {
    _514 :;
    j = 0;
    while(N > j)
    {
      _1026 :;
      sum = *(*(c + (_UINT64)((_UINT64) i)) + (_UINT64)((_UINT64) j)) + sum;
      j = j + 1;
      _770 :;
    }
    goto _1282;
    _1282 :;
    i = i + 1;
    _258 :;
  }
  goto _1538;
  _1538 :;
  printf("sum of c[i][j] is %f\n", sum);
  return;
} /* verify */

