/*******************************************************
 * C file translated from WHIRL Thu Dec  6 15:39:20 2012
 *******************************************************/

/* Include file-level type and variable decls */
#include "fourierf.w2c.h"


__inline _INT32 pthread_equal(
  _UINT64 __thread1,
  _UINT64 __thread2)
{
  
  
  return __thread1 == __thread2;
} /* pthread_equal */


static void CheckPointer(
  void * p,
  _INT8 * name)
{
  
  
  if((_UINT64)(p) == 0ULL)
  {
    printf("Error in fft_float():  %s == NULL\n", name);
    exit(1);
  }
  return;
} /* CheckPointer */


extern void fft_float(
  _UINT32 __NumSamples,
  _INT32 __InverseTransform,
  _IEEE32 * __RealIn,
  _IEEE32 * __ImagIn,
  _IEEE32 * __RealOut,
  _IEEE32 * __ImagOut)
{
  
  register _INT32 _w2c___comma;
  register _UINT32 _w2c___comma0;
  register _INT32 _w2c___ompv_ok_to_fork;
  register _UINT64 _w2c_reg3;
  register _UINT32 _w2c___comma1;
  register _IEEE32 _w2c___cselect;
  register _IEEE64 _w2c___comma2;
  register _IEEE64 _w2c___comma3;
  register _IEEE64 _w2c___comma4;
  register _INT32 _w2c_trip_count;
  register _INT32 _w2c___ompv_ok_to_fork0;
  _IEEE64 delta_angle;
  _IEEE64 denom;
  _INT32 __localized_common_i;
  _INT32 __localized_common_j;
  _INT32 _w2c___localized_common_i0;
  _IEEE64 __localized_common_ar[3LL];
  _IEEE64 __localized_common_ai[3LL];
  _INT32 _w2c___localized_common_j0;
  _INT32 __localized_common_n;
  _INT32 __ompv_gtid_s1;
  
  /*Begin_of_nested_PU(s)*/
  
  NumSamples = __NumSamples;
  InverseTransform = __InverseTransform;
  * RealIn = *__RealIn;
  * ImagIn = *__ImagIn;
  * RealOut = *__RealOut;
  * ImagOut = *__ImagOut;
  _w2c___comma = IsPowerOfTwo(NumSamples);
  if(_w2c___comma == 0)
  {
    printf("Error in fft():  NumSamples=%u is not power of two\n", NumSamples);
    exit(1);
  }
  if(InverseTransform != 0)
  {
    angle_numerator = -angle_numerator;
  }
  CheckPointer(RealIn, "RealIn");
  CheckPointer(RealOut, "RealOut");
  CheckPointer(ImagOut, "ImagOut");
  _w2c___comma0 = NumberOfBitsNeeded(NumSamples);
  NumBits = _w2c___comma0;
  _w2c___ompv_ok_to_fork = 1;
  if(_w2c___ompv_ok_to_fork)
  {
    _w2c___ompv_ok_to_fork = __ompc_can_fork();
  }
  if(_w2c___ompv_ok_to_fork)
  {
    __ompc_fork(0, &__omprg_fft_float_1, _w2c_reg3);
  }
  else
  {
    __ompv_gtid_s1 = __ompc_get_local_thread_num();
    __ompc_serialized_parallel();
    for(__localized_common_i = 0; __localized_common_i < (_INT32) NumSamples; __localized_common_i = __localized_common_i + 1)
    {
      _w2c___comma1 = ReverseBits((_UINT32) __localized_common_i, NumBits);
      __localized_common_j = (_INT32)(_w2c___comma1);
      * (RealOut + (_UINT64)((_UINT64) __localized_common_j)) = *(RealIn + (_UINT64)((_UINT64) __localized_common_i));
      if((_UINT64)(ImagIn) != 0ULL)
      {
        _w2c___cselect = *(ImagIn + (_UINT64)((_UINT64) __localized_common_i));
      }
      else
      {
        _w2c___cselect = 0.0F;
      }
      * (ImagOut + (_UINT64)((_UINT64) __localized_common_j)) = _w2c___cselect;
    }
    __ompc_end_serialized_parallel();
  }
  _w2c___comma2 = log2((_IEEE64)(NumSamples));
  NumIter = _U4F8TRUNC(_w2c___comma2);
  m = 1;
  while(NumIter >= (_UINT32) m)
  {
    _514 :;
    _w2c___comma3 = pow(2.0, (_IEEE64)(m));
    BlockSize = _U4F8TRUNC(_w2c___comma3);
    _w2c___comma4 = pow(2.0, (_IEEE64)(m + -1));
    BlockEnd = _U4F8TRUNC(_w2c___comma4);
    delta_angle = angle_numerator / (_IEEE64)(BlockSize);
    sm2 = sin(delta_angle * -2.0);
    sm1 = sin(-delta_angle);
    cm2 = cos(delta_angle * -2.0);
    cm1 = cos(delta_angle);
    w = cm1 * 2.0;
    _w2c_trip_count = (((_INT32) NumSamples + (_INT32) BlockSize) + -1) / (_INT32) BlockSize;
    _w2c___ompv_ok_to_fork0 = _w2c_trip_count > 1;
    if(_w2c___ompv_ok_to_fork0)
    {
      _w2c___ompv_ok_to_fork0 = __ompc_can_fork();
    }
    if(_w2c___ompv_ok_to_fork0)
    {
      __ompc_fork(0, &__ompdo_fft_float_11, _w2c_reg3);
    }
    else
    {
      __ompv_gtid_s1 = __ompc_get_local_thread_num();
      __ompc_serialized_parallel();
      for(_w2c___localized_common_i0 = 0; _w2c___localized_common_i0 < (_INT32) NumSamples; _w2c___localized_common_i0 = _w2c___localized_common_i0 + (_INT32) BlockSize)
      {
        (__localized_common_ar)[2] = cm2;
        (__localized_common_ar)[1] = cm1;
        (__localized_common_ai)[2] = sm2;
        (__localized_common_ai)[1] = sm1;
        _w2c___localized_common_j0 = _w2c___localized_common_i0;
        __localized_common_n = 0;
        while((_UINT32) __localized_common_n < BlockEnd)
        {
          _1026 :;
          (__localized_common_ar)[0] = ((__localized_common_ar)[1] * w) - (__localized_common_ar)[2];
          (__localized_common_ar)[2] = (__localized_common_ar)[1];
          (__localized_common_ar)[1] = (__localized_common_ar)[0];
          (__localized_common_ai)[0] = ((__localized_common_ai)[1] * w) - (__localized_common_ai)[2];
          (__localized_common_ai)[2] = (__localized_common_ai)[1];
          (__localized_common_ai)[1] = (__localized_common_ai)[0];
          k = (_INT32)((_UINT32) _w2c___localized_common_j0 + BlockEnd);
          tr = ((_IEEE64)(*(RealOut + (_UINT64)((_UINT64) k))) * (__localized_common_ar)[0]) - ((_IEEE64)(*(ImagOut + (_UINT64)((_UINT64) k))) * (__localized_common_ai)[0]);
          ti = ((_IEEE64)(*(RealOut + (_UINT64)((_UINT64) k))) * (__localized_common_ai)[0]) + ((_IEEE64)(*(ImagOut + (_UINT64)((_UINT64) k))) * (__localized_common_ar)[0]);
          * (RealOut + (_UINT64)((_UINT64) k)) = (_IEEE32)((_IEEE64)(*(RealOut + (_UINT64)((_UINT64) _w2c___localized_common_j0))) - tr);
          * (ImagOut + (_UINT64)((_UINT64) k)) = (_IEEE32)((_IEEE64)(*(ImagOut + (_UINT64)((_UINT64) _w2c___localized_common_j0))) - ti);
          * (RealOut + (_UINT64)((_UINT64) _w2c___localized_common_j0)) = (_IEEE32)((_IEEE64)(*(RealOut + (_UINT64)((_UINT64) _w2c___localized_common_j0))) + tr);
          * (ImagOut + (_UINT64)((_UINT64) _w2c___localized_common_j0)) = (_IEEE32)((_IEEE64)(*(ImagOut + (_UINT64)((_UINT64) _w2c___localized_common_j0))) + ti);
          _w2c___localized_common_j0 = _w2c___localized_common_j0 + 1;
          __localized_common_n = __localized_common_n + 1;
          _770 :;
        }
        goto _1282;
        _1282 :;
      }
      __ompc_end_serialized_parallel();
    }
    m = m + 1;
    _258 :;
  }
  goto _1538;
  _1538 :;
  if(InverseTransform != 0)
  {
    denom = (_IEEE64)(NumSamples);
    i = 0;
    while(NumSamples > (_UINT32) i)
    {
      _2050 :;
      * (RealOut + (_UINT64)((_UINT64) i)) = (_IEEE32)((_IEEE64)(*(RealOut + (_UINT64)((_UINT64) i))) / denom);
      * (ImagOut + (_UINT64)((_UINT64) i)) = (_IEEE32)((_IEEE64)(*(ImagOut + (_UINT64)((_UINT64) i))) / denom);
      i = i + 1;
      _1794 :;
    }
    _2306 :;
  }
  return;
} /* fft_float */


static void __omprg_fft_float_1(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT64 __ompv_slink_a;
{
  
  register _INT32 _w2c_temp_limit;
  register _INT32 _w2c_temp_base;
  register _UINT32 _w2c___comma;
  register _IEEE32 _w2c___cselect;
  _UINT64 _temp___slink_sym0;
  _INT32 __ompv_temp_gtid;
  _INT32 __mplocal___localized_common_j;
  _INT32 __mplocal___localized_common_i;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _INT32 __ompv_temp_last_iter;
  
  /*Begin_of_nested_PU(s)*/
  
  _temp___slink_sym0 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  _w2c_temp_limit = (_INT32) NumSamples + -1;
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
    _w2c___comma = ReverseBits((_UINT32) __mplocal___localized_common_i, NumBits);
    __mplocal___localized_common_j = (_INT32)(_w2c___comma);
    * (RealOut + (_UINT64)((_UINT64) __mplocal___localized_common_j)) = *(RealIn + (_UINT64)((_UINT64) __mplocal___localized_common_i));
    if((_UINT64)(ImagIn) != 0ULL)
    {
      _w2c___cselect = *(ImagIn + (_UINT64)((_UINT64) __mplocal___localized_common_i));
    }
    else
    {
      _w2c___cselect = 0.0F;
    }
    * (ImagOut + (_UINT64)((_UINT64) __mplocal___localized_common_j)) = _w2c___cselect;
  }
  __ompc_task_exit();
  return;
} /* __omprg_fft_float_1 */


static void __ompdo_fft_float_11(__ompv_gtid_a, __ompv_slink_a)
  _INT32 __ompv_gtid_a;
  _UINT64 __ompv_slink_a;
{
  
  register _INT32 _w2c_temp_limit;
  register _INT32 _w2c_temp_base;
  _UINT64 _temp___slink_sym1;
  _INT32 __ompv_temp_gtid;
  _INT32 __mplocal___localized_common_n;
  _INT32 __mplocal___localized_common_j;
  _IEEE64 __mplocal___localized_common_ai[3LL];
  _IEEE64 __mplocal___localized_common_ar[3LL];
  _INT32 __mplocal___localized_common_i;
  _INT32 __ompv_temp_do_upper0;
  _INT32 __ompv_temp_do_lower0;
  _INT32 __ompv_temp_do_stride0;
  _INT32 __ompv_temp_last_iter;
  
  /*Begin_of_nested_PU(s)*/
  
  _temp___slink_sym1 = __ompv_slink_a;
  __ompv_temp_gtid = __ompv_gtid_a;
  _w2c_temp_limit = (_INT32) NumSamples + -1;
  _w2c_temp_base = 0;
  __ompv_temp_do_upper0 = _w2c_temp_limit;
  __ompv_temp_do_lower0 = 0;
  __ompv_temp_last_iter = 0;
  __ompc_static_init_4(__ompv_temp_gtid, 2, &__ompv_temp_do_lower0, &__ompv_temp_do_upper0, &__ompv_temp_do_stride0, (_INT32) BlockSize, 1);
  if(__ompv_temp_do_upper0 > _w2c_temp_limit)
  {
    __ompv_temp_do_upper0 = _w2c_temp_limit;
  }
  for(__mplocal___localized_common_i = __ompv_temp_do_lower0; __mplocal___localized_common_i <= __ompv_temp_do_upper0; __mplocal___localized_common_i = __mplocal___localized_common_i + (_INT32) BlockSize)
  {
    (__mplocal___localized_common_ar)[2] = cm2;
    (__mplocal___localized_common_ar)[1] = cm1;
    (__mplocal___localized_common_ai)[2] = sm2;
    (__mplocal___localized_common_ai)[1] = sm1;
    __mplocal___localized_common_j = __mplocal___localized_common_i;
    __mplocal___localized_common_n = 0;
    while((_UINT32) __mplocal___localized_common_n < BlockEnd)
    {
      _259 :;
      (__mplocal___localized_common_ar)[0] = ((__mplocal___localized_common_ar)[1] * w) - (__mplocal___localized_common_ar)[2];
      (__mplocal___localized_common_ar)[2] = (__mplocal___localized_common_ar)[1];
      (__mplocal___localized_common_ar)[1] = (__mplocal___localized_common_ar)[0];
      (__mplocal___localized_common_ai)[0] = ((__mplocal___localized_common_ai)[1] * w) - (__mplocal___localized_common_ai)[2];
      (__mplocal___localized_common_ai)[2] = (__mplocal___localized_common_ai)[1];
      (__mplocal___localized_common_ai)[1] = (__mplocal___localized_common_ai)[0];
      k = (_INT32)((_UINT32) __mplocal___localized_common_j + BlockEnd);
      tr = ((_IEEE64)(*(RealOut + (_UINT64)((_UINT64) k))) * (__mplocal___localized_common_ar)[0]) - ((_IEEE64)(*(ImagOut + (_UINT64)((_UINT64) k))) * (__mplocal___localized_common_ai)[0]);
      ti = ((_IEEE64)(*(RealOut + (_UINT64)((_UINT64) k))) * (__mplocal___localized_common_ai)[0]) + ((_IEEE64)(*(ImagOut + (_UINT64)((_UINT64) k))) * (__mplocal___localized_common_ar)[0]);
      * (RealOut + (_UINT64)((_UINT64) k)) = (_IEEE32)((_IEEE64)(*(RealOut + (_UINT64)((_UINT64) __mplocal___localized_common_j))) - tr);
      * (ImagOut + (_UINT64)((_UINT64) k)) = (_IEEE32)((_IEEE64)(*(ImagOut + (_UINT64)((_UINT64) __mplocal___localized_common_j))) - ti);
      * (RealOut + (_UINT64)((_UINT64) __mplocal___localized_common_j)) = (_IEEE32)((_IEEE64)(*(RealOut + (_UINT64)((_UINT64) __mplocal___localized_common_j))) + tr);
      * (ImagOut + (_UINT64)((_UINT64) __mplocal___localized_common_j)) = (_IEEE32)((_IEEE64)(*(ImagOut + (_UINT64)((_UINT64) __mplocal___localized_common_j))) + ti);
      __mplocal___localized_common_j = __mplocal___localized_common_j + 1;
      __mplocal___localized_common_n = __mplocal___localized_common_n + 1;
      _515 :;
    }
    _771 :;
  }
  __ompc_task_exit();
  return;
} /* __ompdo_fft_float_11 */

