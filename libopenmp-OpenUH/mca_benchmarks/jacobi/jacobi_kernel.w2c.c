/*******************************************************
 * C file translated from WHIRL Tue Oct  9 10:17:54 2012
 *******************************************************/

/* Include file-level type and variable decls */
#include "jacobi_kernel.w2c.h"


__inline _INT32 pthread_equal(
  _UINT64 __thread1,
  _UINT64 __thread2)
{
  
  
  return __thread1 == __thread2;
} /* pthread_equal */


extern _IEEE64 kernel_extern(
  _IEEE64 * x_old,
  _IEEE64 * x,
  _IEEE64 * b,
  _IEEE64 * a,
  _INT32 it_max,
  _INT32 n,
  _IEEE64 diff,
  _IEEE64 diff_tol)
{
  
  register _INT32 _w2c_trip_count;
  register _INT32 _w2c___ompv_ok_to_fork;
  register _UINT64 _w2c_reg3;
  register _INT32 _w2c_trip_count0;
  register _INT32 _w2c___ompv_ok_to_fork0;
  _INT32 __localized_common_i;
  _INT32 _w2c___localized_common_i0;
  _IEEE64 __localized_common_axi;
  _INT32 __localized_common_j;
  _INT32 __ompv_gtid_s1;
  
  /*Begin_of_nested_PU(s)*/
  
  g_x_old = x_old;
  g_x = x;
  g_b = b;
  g_a = a;
  g_it_max = it_max;
  g_n = n;
  g_diff = diff;
  g_diff_tol = diff_tol;
  it = 1;
  while(g_it_max >= it)
  {
    _514 :;
    _w2c_trip_count = g_n;
    _w2c___ompv_ok_to_fork = _w2c_trip_count > 1;
    if(_w2c___ompv_ok_to_fork)
    {
      _w2c___ompv_ok_to_fork = __ompc_can_fork();
    }
    if(_w2c___ompv_ok_to_fork)
    {
      __ompc_fork(0, &__ompdo_kernel_extern_1, _w2c_reg3);
    }
    else
    {
      __ompv_gtid_s1 = __ompc_get_local_thread_num();
      __ompc_serialized_parallel();
      for(__localized_common_i = 0; __localized_common_i < g_n; __localized_common_i = __localized_common_i + 1)
      {
        * (g_x_old + (_UINT64)((_UINT64) __localized_common_i)) = *(g_x + (_UINT64)((_UINT64) __localized_common_i));
      }
      __ompc_end_serialized_parallel();
    }
    _w2c_trip_count0 = g_n;
    _w2c___ompv_ok_to_fork0 = _w2c_trip_count0 > 1;
    if(_w2c___ompv_ok_to_fork0)
    {
      _w2c___ompv_ok_to_fork0 = __ompc_can_fork();
    }
    if(_w2c___ompv_ok_to_fork0)
    {
      __ompc_fork(0, &__ompdo_kernel_extern_2, _w2c_reg3);
    }
    else
    {
      __ompv_gtid_s1 = __ompc_get_local_thread_num();
      __ompc_serialized_parallel();
      for(_w2c___localized_common_i0 = 0; _w2c___localized_common_i0 < g_n; _w2c___localized_common_i0 = _w2c___localized_common_i0 + 1)
      {
        __localized_common_axi = 0.0;
        __localized_common_j = 0;
        while(__localized_common_j < g_n)
        {
          _1026 :;
          __localized_common_axi = __localized_common_axi + (*(g_x_old + (_UINT64)((_UINT64) __localized_common_j)) ** (g_a + (_INT64)(_w2c___localized_common_i0 + (__localized_common_j * g_n))));
          __localized_common_j = __localized_common_j + 1;
          _770 :;
        }
        goto _1282;
        _1282 :;
        * (g_x + (_UINT64)((_UINT64) _w2c___localized_common_i0)) = *(g_x_old + (_UINT64)((_UINT64) _w2c___localized_common_i0)) + ((*(g_b + (_UINT64)((_UINT64) _w2c___localized_common_i0)) - __localized_common_axi) / *(g_a + (_INT64)(_w2c___localized_common_i0 * (g_n + 1))));
      }
      __ompc_end_serialized_parallel();
    }
    g_diff = 0.0;
    i = 0;
    while(g_n > i)
    {
      _1794 :;
      g_diff = g_diff + _F8ABS(*(g_x + (_UINT64)((_UINT64) i)) - *(g_x_old + (_UINT64)((_UINT64) i)));
      i = i + 1;
      _1538 :;
    }
    goto _2050;
    _2050 :;
    if(g_diff <= g_diff_tol)
      goto _2306;
    it = it + 1;
    _258 :;
  }
  goto _2306;
  _2306 :;
  return g_diff;
} /* kernel_extern */


static void __ompdo_kernel_extern_1(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT64 __ompv_slink_a;
{
  
  register _INT32 _w2c_temp_limit;
  register _INT32 _w2c_temp_base;
  _UINT64 _temp___slink_sym0;
  _INT32 __ompv_temp_gtid;
  _INT32 __mplocal___localized_common_i;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _INT32 __ompv_temp_last_iter;
  
  /*Begin_of_nested_PU(s)*/
  
  _temp___slink_sym0 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  _w2c_temp_limit = g_n + -1;
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
    * (g_x_old + (_UINT64)((_UINT64) __mplocal___localized_common_i)) = *(g_x + (_UINT64)((_UINT64) __mplocal___localized_common_i));
  }
  __ompc_task_exit();
  return;
} /* __ompdo_kernel_extern_1 */


static void __ompdo_kernel_extern_2(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT64 __ompv_slink_a;
{
  
  register _INT32 _w2c_temp_limit;
  register _INT32 _w2c_temp_base;
  _UINT64 _temp___slink_sym1;
  _INT32 __ompv_temp_gtid;
  _INT32 __mplocal___localized_common_j;
  _IEEE64 __mplocal___localized_common_axi;
  _INT32 __mplocal___localized_common_i;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _INT32 __ompv_temp_last_iter;
  
  /*Begin_of_nested_PU(s)*/
  
  _temp___slink_sym1 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  _w2c_temp_limit = g_n + -1;
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
    __mplocal___localized_common_axi = 0.0;
    __mplocal___localized_common_j = 0;
    while(__mplocal___localized_common_j < g_n)
    {
      _259 :;
      __mplocal___localized_common_axi = __mplocal___localized_common_axi + (*(g_x_old + (_UINT64)((_UINT64) __mplocal___localized_common_j)) ** (g_a + (_INT64)(__mplocal___localized_common_i + (__mplocal___localized_common_j * g_n))));
      __mplocal___localized_common_j = __mplocal___localized_common_j + 1;
      _515 :;
    }
    _771 :;
    * (g_x + (_UINT64)((_UINT64) __mplocal___localized_common_i)) = *(g_x_old + (_UINT64)((_UINT64) __mplocal___localized_common_i)) + ((*(g_b + (_UINT64)((_UINT64) __mplocal___localized_common_i)) - __mplocal___localized_common_axi) / *(g_a + (_INT64)(__mplocal___localized_common_i * (g_n + 1))));
  }
  __ompc_task_exit();
  return;
} /* __ompdo_kernel_extern_2 */

