/* Include builtin types and operators.*/
/* It is like under $TOOLROOT/include/$REL_VER/ */
#include <whirl2c.h>

/* Types */
/* File-level vars and routines */
__inline _INT32 pthread_equal(_UINT64, _UINT64);

extern void lud_omp(_IEEE32 *, _INT32);

_IEEE32 * a_g;

_INT32 size_g;

_INT32 i;

_INT32 j;

_INT32 k;

_IEEE32 sum;

extern _INT32 __ompc_get_local_thread_num();

extern _INT32 __ompc_serialized_parallel();

extern _INT32 __ompc_end_serialized_parallel();

static void __ompdo_lud_omp_1();

extern _INT32 __ompc_static_init_4();

extern _INT32 __ompc_task_exit();

extern _INT32 __ompc_fork();

extern _INT32 __ompc_can_fork();

static void __ompdo_lud_omp_2();

