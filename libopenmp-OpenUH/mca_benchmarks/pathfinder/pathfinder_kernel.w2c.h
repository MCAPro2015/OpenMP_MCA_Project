/* Include builtin types and operators.*/
/* It is like under $TOOLROOT/include/$REL_VER/ */
#include <whirl2c.h>

/* Types */
/* File-level vars and routines */
__inline _INT32 pthread_equal(_UINT64, _UINT64);

extern void kernel(_INT32, _INT32, _INT32 *, _INT32 *);

_INT32 * src_g;

_INT32 * dst_g;

_INT32 cols_g;

_INT32 t_g;

_INT32 min;

_INT32 n;

extern _INT32 ** wall;

extern _INT32 __ompc_get_local_thread_num();

extern _INT32 __ompc_serialized_parallel();

extern _INT32 __ompc_end_serialized_parallel();

static void __ompdo_kernel_1();

extern _INT32 __ompc_static_init_4();

extern _INT32 __ompc_task_exit();

extern _INT32 __ompc_fork();

extern _INT32 __ompc_can_fork();

