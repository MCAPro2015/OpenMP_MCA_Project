/* Include builtin types and operators.*/
/* It is like under $TOOLROOT/include/$REL_VER/ */
#include <whirl2c.h>

/* Types */
/* File-level vars and routines */
__inline _INT32 pthread_equal(_UINT64, _UINT64);

extern _INT32 main(_INT32, _INT8 **);

extern _INT32 printf(const _INT8 *, ...);

extern void exit(_INT32);

extern _INT32 atoi();

_INT32 N;

extern void * malloc(_UINT64);

_IEEE64 ** a;

_IEEE64 ** b;

_IEEE64 ** c;

_INT32 i;

extern _INT32 omp_get_num_threads(void);

_INT32 threads;

extern void init();

extern _INT32 gettimeofday(struct timeval * restrict, struct timezone * restrict);

extern void mmm();

extern void verify();

_INT32 j;

_INT32 k;

_IEEE64 sum;

static void __omprg_main_1();

extern _INT32 __ompc_master();

extern _INT32 __ompc_end_master();

extern _INT32 __ompc_task_exit();

extern _INT32 __ompc_fork();

extern _INT32 __ompc_get_local_thread_num();

extern _INT32 __ompc_serialized_parallel();

extern _INT32 __ompc_end_serialized_parallel();

extern _INT32 __ompc_can_fork();

static void __omprg_mmm_1();

extern _INT32 __ompc_static_init_4();

extern _INT32 __ompc_barrier();

