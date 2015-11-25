/* Include builtin types and operators.*/
/* It is like under $TOOLROOT/include/$REL_VER/ */
#include <whirl2c.h>

/* Types */
/* File-level vars and routines */
__inline _INT32 pthread_equal(_UINT64, _UINT64);

extern _IEEE64 kernel_extern(_IEEE64 *, _IEEE64 *, _IEEE64 *, _IEEE64 *, _INT32, _INT32, _IEEE64, _IEEE64);

_IEEE64 * g_x_old;

_IEEE64 * g_x;

_IEEE64 * g_b;

_IEEE64 * g_a;

_INT32 g_it_max;

_INT32 g_n;

_IEEE64 g_diff;

_IEEE64 g_diff_tol;

_INT32 it;

_INT32 i;

_IEEE64 axi;

_INT32 j;

extern _INT32 __ompc_get_local_thread_num();

extern _INT32 __ompc_serialized_parallel();

extern _INT32 __ompc_end_serialized_parallel();

static void __ompdo_kernel_extern_1();

extern _INT32 __ompc_static_init_4();

extern _INT32 __ompc_task_exit();

extern _INT32 __ompc_fork();

extern _INT32 __ompc_can_fork();

static void __ompdo_kernel_extern_2();

