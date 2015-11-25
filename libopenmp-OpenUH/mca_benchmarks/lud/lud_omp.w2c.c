/*******************************************************
 * C file translated from WHIRL Mon Oct 29 11:25:15 2012
 *******************************************************/

/* Include file-level type and variable decls */
#include "lud_omp.w2c.h"


__inline _INT32 pthread_equal(
  _UINT64 __thread1,
  _UINT64 __thread2)
{
  
  
  return __thread1 == __thread2;
} /* pthread_equal */


extern void lud_omp(
  _IEEE32 * a,
  _INT32 size)
{
  
  register _INT32 _w2c_trip_count;
  register _INT32 _w2c___ompv_ok_to_fork;
  register _UINT64 _w2c_reg3;
  register _INT32 _w2c_trip_count0;
  register _INT32 _w2c___ompv_ok_to_fork0;
  _INT32 __localized_common_j;
  _IEEE32 __localized_common_sum;
  _INT32 __localized_common_k;
  _INT32 _w2c___localized_common_j0;
  _IEEE32 _w2c___localized_common_sum0;
  _INT32 _w2c___localized_common_k0;
  _INT32 __ompv_gtid_s1;
  
  /*Begin_of_nested_PU(s)*/
  
  a_g = a;
  size_g = size;
  i = 0;
  while(size_g > i)
  {
    _514 :;
    _w2c_trip_count = size_g - i;
    _w2c___ompv_ok_to_fork = _w2c_trip_count > 1;
    if(_w2c___ompv_ok_to_fork)
    {
      _w2c___ompv_ok_to_fork = __ompc_can_fork();
    }
    if(_w2c___ompv_ok_to_fork)
    {
      __ompc_fork(0, &__ompdo_lud_omp_1, _w2c_reg3);
    }
    else
    {
      __ompv_gtid_s1 = __ompc_get_local_thread_num();
      __ompc_serialized_parallel();
      for(__localized_common_j = i; __localized_common_j < size_g; __localized_common_j = __localized_common_j + 1)
      {
        __localized_common_sum = *(a_g + (_INT64)(__localized_common_j + (size_g * i)));
        __localized_common_k = 0;
        while(__localized_common_k < i)
        {
          _1026 :;
          __localized_common_sum = __localized_common_sum - (*(a_g + (_INT64)(__localized_common_j + (__localized_common_k * size_g))) ** (a_g + (_INT64)(__localized_common_k + (size_g * i))));
          __localized_common_k = __localized_common_k + 1;
          _770 :;
        }
        goto _1282;
        _1282 :;
        * (a_g + (_INT64)(__localized_common_j + (size_g * i))) = __localized_common_sum;
      }
      __ompc_end_serialized_parallel();
    }
    _w2c_trip_count0 = (size_g - i) + -1;
    _w2c___ompv_ok_to_fork0 = _w2c_trip_count0 > 1;
    if(_w2c___ompv_ok_to_fork0)
    {
      _w2c___ompv_ok_to_fork0 = __ompc_can_fork();
    }
    if(_w2c___ompv_ok_to_fork0)
    {
      __ompc_fork(0, &__ompdo_lud_omp_2, _w2c_reg3);
    }
    else
    {
      __ompv_gtid_s1 = __ompc_get_local_thread_num();
      __ompc_serialized_parallel();
      for(_w2c___localized_common_j0 = i + 1; _w2c___localized_common_j0 < size_g; _w2c___localized_common_j0 = _w2c___localized_common_j0 + 1)
      {
        _w2c___localized_common_sum0 = *(a_g + (_INT64)(i + (_w2c___localized_common_j0 * size_g)));
        _w2c___localized_common_k0 = 0;
        while(_w2c___localized_common_k0 < i)
        {
          _1794 :;
          _w2c___localized_common_sum0 = _w2c___localized_common_sum0 - (*(a_g + (_INT64)(_w2c___localized_common_k0 + (_w2c___localized_common_j0 * size_g))) ** (a_g + (_INT64)(i + (_w2c___localized_common_k0 * size_g))));
          _w2c___localized_common_k0 = _w2c___localized_common_k0 + 1;
          _1538 :;
        }
        goto _2050;
        _2050 :;
        * (a_g + (_INT64)(i + (_w2c___localized_common_j0 * size_g))) = _w2c___localized_common_sum0 / *(a_g + (_INT64)(i * (size_g + 1)));
      }
      __ompc_end_serialized_parallel();
    }
    i = i + 1;
    _258 :;
  }
  goto _2306;
  _2306 :;
  return;
} /* lud_omp */


static void __ompdo_lud_omp_1(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT64 __ompv_slink_a;
{
  
  register _INT32 _w2c_temp_limit;
  register _INT32 _w2c_temp_base;
  _UINT64 _temp___slink_sym0;
  _INT32 __ompv_temp_gtid;
  _INT32 __mplocal___localized_common_k;
  _IEEE32 __mplocal___localized_common_sum;
  _INT32 __mplocal___localized_common_j;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _INT32 __ompv_temp_last_iter;
  
  /*Begin_of_nested_PU(s)*/
  
  _temp___slink_sym0 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  _w2c_temp_limit = size_g + -1;
  _w2c_temp_base = i;
  __ompv_temp_do_upper0 = _w2c_temp_limit;
  __ompv_temp_do_lower0 = i;
  __ompv_temp_last_iter = 0;
  __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower0, &__ompv_temp_do_upper0, &__ompv_temp_do_stride0, 1, 1);
  if(__ompv_temp_do_upper0 > _w2c_temp_limit)
  {
    __ompv_temp_do_upper0 = _w2c_temp_limit;
  }
  for(__mplocal___localized_common_j = __ompv_temp_do_lower0; __mplocal___localized_common_j <= __ompv_temp_do_upper0; __mplocal___localized_common_j = __mplocal___localized_common_j + 1)
  {
    __mplocal___localized_common_sum = *(a_g + (_INT64)(__mplocal___localized_common_j + (size_g * i)));
    __mplocal___localized_common_k = 0;
    while(__mplocal___localized_common_k < i)
    {
      _259 :;
      __mplocal___localized_common_sum = __mplocal___localized_common_sum - (*(a_g + (_INT64)(__mplocal___localized_common_k + (size_g * i))) ** (a_g + (_INT64)(__mplocal___localized_common_j + (__mplocal___localized_common_k * size_g))));
      __mplocal___localized_common_k = __mplocal___localized_common_k + 1;
      _515 :;
    }
    _771 :;
    * (a_g + (_INT64)(__mplocal___localized_common_j + (size_g * i))) = __mplocal___localized_common_sum;
  }
  __ompc_task_exit();
  return;
} /* __ompdo_lud_omp_1 */


static void __ompdo_lud_omp_2(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT64 __ompv_slink_a;
{
  
  register _INT32 _w2c_temp_limit;
  register _INT32 _w2c_temp_base;
  _UINT64 _temp___slink_sym1;
  _INT32 __ompv_temp_gtid;
  _INT32 __mplocal___localized_common_k;
  _IEEE32 __mplocal___localized_common_sum;
  _INT32 __mplocal___localized_common_j;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _INT32 __ompv_temp_last_iter;
  
  /*Begin_of_nested_PU(s)*/
  
  _temp___slink_sym1 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  _w2c_temp_limit = size_g + -1;
  _w2c_temp_base = i + 1;
  __ompv_temp_do_upper0 = _w2c_temp_limit;
  __ompv_temp_do_lower0 = i + 1;
  __ompv_temp_last_iter = 0;
  __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower0, &__ompv_temp_do_upper0, &__ompv_temp_do_stride0, 1, 1);
  if(__ompv_temp_do_upper0 > _w2c_temp_limit)
  {
    __ompv_temp_do_upper0 = _w2c_temp_limit;
  }
  for(__mplocal___localized_common_j = __ompv_temp_do_lower0; __mplocal___localized_common_j <= __ompv_temp_do_upper0; __mplocal___localized_common_j = __mplocal___localized_common_j + 1)
  {
    __mplocal___localized_common_sum = *(a_g + (_INT64)(i + (__mplocal___localized_common_j * size_g)));
    __mplocal___localized_common_k = 0;
    while(__mplocal___localized_common_k < i)
    {
      _259 :;
      __mplocal___localized_common_sum = __mplocal___localized_common_sum - (*(a_g + (_INT64)(__mplocal___localized_common_k + (__mplocal___localized_common_j * size_g))) ** (a_g + (_INT64)(i + (__mplocal___localized_common_k * size_g))));
      __mplocal___localized_common_k = __mplocal___localized_common_k + 1;
      _515 :;
    }
    _771 :;
    * (a_g + (_INT64)(i + (__mplocal___localized_common_j * size_g))) = __mplocal___localized_common_sum / *(a_g + (_INT64)(i * (size_g + 1)));
  }
  __ompc_task_exit();
  return;
} /* __ompdo_lud_omp_2 */

