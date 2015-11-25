/* Include builtin types and operators.*/
/* It is like under $TOOLROOT/include/$REL_VER/ */
#include <whirl2c.h>

/* Types */
/* File-level vars and routines */
__inline _INT32 pthread_equal(_UINT64, _UINT64);

static void CheckPointer(void *, _INT8 *);

extern _INT32 printf(const _INT8 *, ...);

extern void exit(_INT32);

extern void fft_float(_UINT32, _INT32, _IEEE32 *, _IEEE32 *, _IEEE32 *, _IEEE32 *);

_UINT32 NumSamples;

_INT32 InverseTransform;

_IEEE32 * RealIn;

_IEEE32 * ImagIn;

_IEEE32 * RealOut;

_IEEE32 * ImagOut;

extern _INT32 IsPowerOfTwo(_UINT32);

_IEEE64 angle_numerator = 6.283185307179586232;

extern _UINT32 NumberOfBitsNeeded(_UINT32);

_UINT32 NumBits;

_INT32 j;

_INT32 i;

extern _UINT32 ReverseBits(_UINT32, _UINT32);

extern _IEEE64 log2(_IEEE64);

_UINT32 NumIter;

_INT32 m;

extern _IEEE64 pow(_IEEE64, _IEEE64);

_UINT32 BlockSize;

_UINT32 BlockEnd;

_IEEE64 sm2;

_IEEE64 sm1;

_IEEE64 cm2;

_IEEE64 cm1;

_IEEE64 w;

_INT32 n;

_IEEE64 ar[3LL];

_IEEE64 ai[3LL];

_INT32 k;

_IEEE64 tr;

_IEEE64 ti;

_UINT32 sum;

_IEEE64 temp;

static void __omprg_fft_float_1();

extern _INT32 __ompc_static_init_4();

extern _INT32 __ompc_task_exit();

extern _INT32 __ompc_fork();

extern _INT32 __ompc_get_local_thread_num();

extern _INT32 __ompc_serialized_parallel();

extern _INT32 __ompc_end_serialized_parallel();

extern _INT32 __ompc_can_fork();

static void __ompdo_fft_float_11();

