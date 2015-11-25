/* Include builtin types and operators.*/
/* It is like under $TOOLROOT/include/$REL_VER/ */
#include <whirl2c.h>

/* Types */
/* File-level vars and routines */
__inline _INT32 pthread_equal(_UINT64, _UINT64);

extern void dowork();

extern _INT32 omp_get_thread_num(void);

_INT32 me;

extern _INT32 omp_get_num_threads(void);

extern _INT32 nth;

extern _INT32 nv;

extern _INT32 printf(const _INT8 *, ...);

extern void exit(_INT32);

extern _INT32 chunk;

_INT32 startv;

_INT32 endv;

_INT32 step;

extern _INT32 largeint;

extern _INT32 md;

extern _INT32 mv;

extern void findmymin();

_UINT32 mymd;

_INT32 mymv;

extern _INT32 * notdone;

extern void updatemind();

_INT64 __mplock_0;

extern _INT32 __ompc_critical();

extern _INT32 __ompc_end_critical();

static void __omprg_dowork_1();

extern _INT32 __ompc_master();

extern _INT32 __ompc_end_master();

extern _INT32 __ompc_ebarrier();

extern _INT32 __ompc_task_exit();

extern _INT32 __ompc_fork();

extern _INT32 __ompc_get_local_thread_num();

extern _INT32 __ompc_serialized_parallel();

extern _INT32 __ompc_end_serialized_parallel();

extern _INT32 __ompc_can_fork();

