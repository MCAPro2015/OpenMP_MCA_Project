/*******************************************************
 * C file translated from WHIRL Thu Nov  1 13:29:12 2012
 *******************************************************/

/* Include file-level type and variable decls */
#include "pathfinder_kernel.w2c.h"


__inline _INT32 pthread_equal(
  _UINT64 __thread1,
  _UINT64 __thread2)
{
  
  
  return __thread1 == __thread2;
} /* pthread_equal */


extern void kernel(
  _INT32 t,
  _INT32 cols,
  _INT32 * src,
  _INT32 * dst)
{
  
  register _INT32 _w2c_trip_count;
  register _INT32 _w2c___ompv_ok_to_fork;
  register _UINT64 _w2c_reg3;
  _INT32 __localized_common_n;
  _INT32 __localized_common_min;
  _INT32 __ompv_gtid_s1;
  
  /*Begin_of_nested_PU(s)*/
  
  src_g = src;
  dst_g = dst;
  cols_g = cols;
  t_g = t;
  _w2c_trip_count = cols_g;
  _w2c___ompv_ok_to_fork = _w2c_trip_count > 1;
  if(_w2c___ompv_ok_to_fork)
  {
    _w2c___ompv_ok_to_fork = __ompc_can_fork();
  }
  if(_w2c___ompv_ok_to_fork)
  {
    __ompc_fork(0, &__ompdo_kernel_1, _w2c_reg3);
  }
  else
  {
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    __ompc_serialized_parallel();
    for(__localized_common_n = 0; __localized_common_n < cols_g; __localized_common_n = __localized_common_n + 1)
    {
      __localized_common_min = *(src_g + (_UINT64)((_UINT64) __localized_common_n));
      if(__localized_common_n > 0)
      {
        __localized_common_min = _I4MIN(*((_INT32 *)(((_UINT64)((_UINT64) __localized_common_n) * 4ULL) + ((_UINT64)(src_g) - 4ULL))), __localized_common_min);
      }
      if(__localized_common_n < (cols_g + -1))
      {
        __localized_common_min = _I4MIN(*((src_g + (_UINT64)((_UINT64) __localized_common_n)) + 1LL), __localized_common_min);
      }
      * (dst_g + (_UINT64)((_UINT64) __localized_common_n)) = *(*((wall + (_UINT64)((_UINT64) t_g)) + 1LL) + (_UINT64)((_UINT64) __localized_common_n)) + __localized_common_min;
    }
    __ompc_end_serialized_parallel();
  }
  return;
} /* kernel */


static void __ompdo_kernel_1(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT64 __ompv_slink_a;
{
  
  register _INT32 _w2c_temp_limit;
  register _INT32 _w2c_temp_base;
  _UINT64 _temp___slink_sym0;
  _INT32 __ompv_temp_gtid;
  _INT32 __mplocal___localized_common_min;
  _INT32 __mplocal___localized_common_n;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _INT32 __ompv_temp_last_iter;
  
  /*Begin_of_nested_PU(s)*/
  
  _temp___slink_sym0 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  _w2c_temp_limit = cols_g + -1;
  _w2c_temp_base = 0;
  __ompv_temp_do_upper0 = _w2c_temp_limit;
  __ompv_temp_do_lower0 = 0;
  __ompv_temp_last_iter = 0;
  __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower0, &__ompv_temp_do_upper0, &__ompv_temp_do_stride0, 1, 1);
  if(__ompv_temp_do_upper0 > _w2c_temp_limit)
  {
    __ompv_temp_do_upper0 = _w2c_temp_limit;
  }
  for(__mplocal___localized_common_n = __ompv_temp_do_lower0; __mplocal___localized_common_n <= __ompv_temp_do_upper0; __mplocal___localized_common_n = __mplocal___localized_common_n + 1)
  {
    __mplocal___localized_common_min = *(src_g + (_UINT64)((_UINT64) __mplocal___localized_common_n));
    if(__mplocal___localized_common_n > 0)
    {
      __mplocal___localized_common_min = _I4MIN(*((_INT32 *)(((_UINT64)((_UINT64) __mplocal___localized_common_n) * 4ULL) + ((_UINT64)(src_g) - 4ULL))), __mplocal___localized_common_min);
    }
    if(__mplocal___localized_common_n < (cols_g + -1))
    {
      __mplocal___localized_common_min = _I4MIN(*((src_g + (_UINT64)((_UINT64) __mplocal___localized_common_n)) + 1LL), __mplocal___localized_common_min);
    }
    * (dst_g + (_UINT64)((_UINT64) __mplocal___localized_common_n)) = *(*((wall + (_UINT64)((_UINT64) t_g)) + 1LL) + (_UINT64)((_UINT64) __mplocal___localized_common_n)) + __mplocal___localized_common_min;
  }
  __ompc_task_exit();
  return;
} /* __ompdo_kernel_1 */

