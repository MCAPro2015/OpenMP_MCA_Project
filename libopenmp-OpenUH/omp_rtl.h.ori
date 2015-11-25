/* File : omp_rtl.c
 * Contents : the execution unit and team data structure, internal function prototypes of the OpenMP RTL routine using MCA
 * Contributor : Besar Wicaksono
 *								Cheng Wang --after Jan. 2012
 */

#ifndef OMP_RTL_H
#define OMP_RTL_H
#include <mrapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp_type.h>

/* Domain and node id for the master thread */
#ifndef DOMAIN_NODE
#define DOMAIN 0
#define NODE 0
#endif

/* machine dependent values*/
#define CACHE_LINE_SIZE		64	// L1D cache line size, was 256 by Cheng

#ifdef TARG_IA64
#define CACHE_LINE_SIZE_L2L3    256    // L2L3 cache line size, was 256
#else
#define CACHE_LINE_SIZE_L2L3     256      // L2L3 cache line size was 256
#endif

#define OMP_NESTED_DEFAULT	0	
#define OMP_DYNAMIC_DEFAULT	0
#define OMP_MAX_NUM_THREADS	8     //Cheng --05/14 from 8 to 2
//#define OMP_STACK_SIZE_DEFAULT	0x400000L /* 4MB*/
#define OMP_STACK_SIZE_DEFAULT	0x020000L //Cheng --05/14 /*4K*/ 
#define OMP_NEW_WORK_SLEEP 500 //was 1000 -- Cheng
#define OMP_NEW_NODE_SLEEP 1000 //was 1000 -- Cheng
#define OMP_MCA_SEM_KEY_OFFSET 0x10000000
#define OMP_MCA_MUT_KEY_BARRIER_OFFSET 0x01000000
#define OMP_MCA_MUT_KEY_SCHEDULING_OFFSET 0x00100000
#define OMP_MCA_MUT_KEY_CRITICAL_OFFSET 0x00010000
#define OMP_MCA_MUT_KEY_SINGLE_OFFSET 0x00001000
#define OMP_MCA_MUT_KEY_REDUCTION_OFFSET 0x00000100
#define OMP_MCA_MUT_KEY_RWLCK_OFFSET 0x00000010

#define OMP_NESTED_DEPTH 1
//#define OMP_SHMEM_SIZE 512*1000000 /* 10MB */
#define OMP_SHMEM_SIZE 1024*1024*2  //in byte
//#define OMP_SHMEM_NODE 65536
#define OMP_SHMEM_NODE 32
#define OMP_SHMEM_META_KEY 0xCABE0000
#define OMP_SHMEM_DATA_KEY 0xBABE0000
#define ALIGN_SIZE 64

/* Waiting while condition is true */
//#define MAX_COUNT 50000
#define MAX_COUNT 1000000000
#define OMPC_WAIT_WHILE(condition) \
      { \
          if (condition) { \
              int count = 0; \
              while (condition) { \
                   if (count > MAX_COUNT) { \
	                count = 0; \
                   } \
		   count++; \
              } \
          } \
      }

typedef void* frame_pointer_t;
typedef void (*omp_micro)(int, frame_pointer_t);
typedef void* (*pthread_entry)(void *);
typedef struct omp_icv omp_icv_t;
typedef struct omp_v_thread omp_v_thread_t;
typedef struct omp_team omp_team_t;
typedef struct omp_mca_node omp_mca_node_t;
typedef struct shared_memory_slot shared_memory_slot_t;
typedef struct ompc_lock ompc_lock_t;

/* global variables used in RTL, declared in omp_thread.c */
extern volatile int __omp_nested;
extern volatile int __omp_dynamic;
extern volatile int __omp_max_num_threads;
extern volatile int __omp_nthreads_var;
extern volatile unsigned long int __omp_stack_size;
extern volatile int __omp_shmem_size;
extern volatile int __omp_shmem_node;

extern int __omp_num_cpus;
extern int __omp_wait_count;
//extern int is_finished;

// omp_mca_node_t* list_nodes;
extern __thread mrapi_shmem_hndl_t shmem_meta;
extern __thread shmem_data;   //comment by Cheng at 04/12
//extern __thread mrapi_shmem_hndl_t shmem_data;  //added by Cheng at 04/12

extern __thread shared_memory_slot_t* addr_meta_root;
extern __thread void* addr_data_root;

/*OpenMP lock*/
/*struct ompc_lock {
  int flag;
  union{
    pthread_spinlock_t spin_data;
    pthread_mutex_t mutex_data;
  } lock;
};
*/

struct {
  int __attribute__ ((__aligned__(ALIGN_SIZE))) flag;
  union{
    pthread_spinlock_t spin_data;
    pthread_mutex_t mutex_data;
  } lock;
}__attribute__ ((__aligned__(ALIGN_SIZE))) ompc_lock;

/* OpenMP execution mode */
typedef enum {
    OMP_EXE_MODE_SLEEP, /* node is waiting for work */
    OMP_EXE_MODE_SEQUENTIAL,    /* node is in serial execution --master node only-- */
    OMP_EXE_MODE_NORMAL,    /* node is doing parallel job in the first level of parallel region */
    OMP_EXE_MODE_NESTED,    /* node is doing parallel job in a nested parallel region */
    OMP_EXE_MODE_NESTED_SEQUENTIAL, /* node is doing serial execution in a nested parallel region (because no more node is available) */
    
    OMP_EXE_MODE_DEFAULT = OMP_EXE_MODE_SEQUENTIAL
} omp_exe_mode_t;

/*
*  ICV variables
*/
struct omp_icv{
	/*volatile int __omp_nested;
	volatile int __omp_dynamic;
	volatile int __omp_max_num_threads;
	volatile int __omp_nthreads_var;
	volatile unsigned long int __omp_stack_size;
	volatile int __omp_shmem_size;
	volatile int __omp_shmem_node;
	*/
    int __omp_nested;
	int __omp_dynamic;
	int __omp_max_num_threads;
	int __omp_nthreads_var;
	unsigned long int __omp_stack_size;
	int __omp_shmem_size;
	int  __omp_shmem_node;

	int __omp_num_cpus;
	volatile int is_finished;
	// omp_mca_node_t* list_nodes;
	unsigned long offset_list_nodes;
};

/* MCA Node
*  in shared memory, domain id is always 0 (maybe)
*/
struct omp_mca_node{
	pthread_t pthread_handle;
	int domain_id;
	int node_id;
	int num_team;	
    // omp_team_t** list_team;	//the teams that the node is currently involved
    unsigned long offset_list_team;
    // int* list_vthread_id;
    unsigned long offset_list_vthread_id;
    volatile int new_work;
	// char *stack_pointer;
	unsigned long offset_stack_pointer;
    volatile omp_exe_mode_t omp_exe_mode;
    int create_new_node;
    volatile int is_ready;
	
	int loop_count;
	long schedule_count;
	long ordered_count;
	long rest_iter_count;
} __attribute__ ((__aligned__(CACHE_LINE_SIZE_L2L3)));

/* OpenMP team */
struct omp_team{
	int team_id;
	int total_team_size;
    int active_team_size;
	int team_level;
	int is_nested;
    volatile int count_barrier;
	int schedule_type;
	int num_param;
	// void** param_index;
	unsigned long offset_param_index;
    volatile int loop_count;
	volatile long schedule_count;
	volatile long ordered_count;
	volatile long loop_lower_bound;
	long loop_increament;
	long loop_upper_bound;
	long loop_increment;
	long chunk_size;
	
    volatile omp_micro entry_func;
    volatile frame_pointer_t frame_pointer;
    
    /* turnstile : a semaphore being locked and unlocked rapid succession */
//     mrapi_sem_hndl_t turnstile;
//     mrapi_mutex_hndl_t mutex_barrier;
//     mrapi_key_t lock_key_barrier;
// 	mrapi_mutex_hndl_t mutex_scheduling;
//     mrapi_key_t lock_key_scheduling;

	// omp_team_t* next;	//points to the team of the next level parallel region --currently not used, focus on basic parallel regions--
	// omp_v_thread_t* list_vthread;	//array of vthreads
	unsigned long offset_list_vthread;
} __attribute__ ((__aligned__(CACHE_LINE_SIZE_L2L3)));

/* OpenMP thread 
(for now there is no support for nested work sharing construct, the mapping of node-vthread is 1 to 1.
If nested is enabled, we may need to move some variables from node structure to vthread structure.
*/
struct omp_v_thread {
	int node_id;	//the node executes this thread
	int vthread_id;
	
} __attribute__ ((__aligned__(CACHE_LINE_SIZE)));

 struct shared_memory_slot{
//    void *addr;
   unsigned long offset;
   unsigned long size;
   unsigned short feb; //full empty bit. 1=full
   unsigned short available;
   unsigned long offset_next;
   unsigned long offset_prev;
//    shared_memory_slot_t *next;
//    shared_memory_slot_t *prev;
};

typedef enum {
    OMP_SCHED_UNKNOWN     = 0,
    OMP_SCHED_STATIC      = 1,
    OMP_SCHED_STATIC_EVEN = 2,
    OMP_SCHED_DYNAMIC     = 3,
    OMP_SCHED_GUIDED      = 4,
    OMP_SCHED_RUNTIME     = 5,
    
    OMP_SCHED_ORDERED_UNKNOWN     = 32,
    OMP_SCHED_ORDERED_STATIC      = 33,
    OMP_SCHED_ORDERED_STATIC_EVEN = 34,
    OMP_SCHED_ORDERED_DYNAMIC     = 35,
    OMP_SCHED_ORDERED_GUIDED      = 36,
    OMP_SCHED_ORDERED_RUNTIME     = 37,
    
    OMP_SCHED_DEFAULT = OMP_SCHED_STATIC_EVEN
} omp_sched_t;

extern volatile omp_sched_t __omp_rt_sched_type;
extern int __omp_rt_sched_size;

#define OMP_SCHEDULE_DEFAULT	2
#define OMP_CHUNK_SIZE_DEFAULT	1
#define OMP_SCHED_ORDERED_GAP	32

int __attribute__ ((constructor)) __ompc_init_rtl();
void __ompc_fini_rtl();
inline void *__ompc_slave_routine(void *_u_thread_id);
inline void __ompc_fork(const int _num_threads, omp_micro micro_task,frame_pointer_t frame_pointer);
inline int __ompc_can_fork();
inline void __ompc_ebarrier(void);
inline void __ompc_barrier(void);
void __ompc_level_1_barrier(const int vthread_id);
int __ompc_get_local_thread_num();
void __ompc_serialized_parallel(int vthread_id);
void __ompc_end_serialized_parallel(int vthread_id);
int __ompc_check_num_threads(const int _num_threads);
int __ompc_create_mca_node(int domain_id,int node_id);
void __ompc_static_init_4 (int global_tid, omp_sched_t schedtype,int *plower,int *pupper, int *pstride,int incr, int chunk);
void __ompc_scheduler_init_4 (int global_tid, omp_sched_t schedtype, int lower, int upper,int stride, int chunk);
int __ompc_schedule_next_4 (int global_tid, int *plower, int *pupper, int *pstride);
extern void __ompc_task_exit();

/*shared memory*/
unsigned long __ompc_allocate(size_t size);
void __ompc_deallocate(void* p);
void __ompc_deallocate_offset(unsigned long offset);
void __ompc_setup_param_offset(int num_param);
void __ompc_fill_param_offset(int idx, void* addr);
void* __ompc_get_param_offset(int idx);

/*lock*/
inline void __ompc_critical(int gtid, volatile ompc_lock_t **lck);
inline void __ompc_end_critical(int gtid, volatile ompc_lock_t  **lck);

/*reduction*/
inline void __ompc_reduction(int gtid, volatile ompc_lock_t **lck);
inline void __ompc_end_reduction(int gtid, volatile ompc_lock_t  **lck);


omp_int32 __ompc_single(omp_int32 global_tid);
void __ompc_end_single(omp_int32 global_tid);

omp_int32 __ompc_master(omp_int32 global_tid);
void __ompc_end_master(omp_int32 global_tid);


/* Function prototypes for getting/setting ICVs and runtime information */
// int __ompc_can_fork(void);
// void __ompc_set_nested(const int __nested);
// void __ompc_set_dynamic(const int __dynamic);
// int __ompc_get_dynamic(void);
// int __ompc_get_nested(void);
// int __ompc_get_max_threads(void);
// int __ompc_get_num_procs(void);
// void __ompc_set_num_threads(const int __num_threads);
// int __ompc_in_parallel(void);
// int __ompc_get_local_thread_num(void);
// int __ompc_get_num_threads(void);


// tournament barrier data structures

#define OMP_LOG_MAX_NUM_THREADS 3 

#define False ((boolean) 0)
#define True ((boolean) 1)
typedef unsigned char role_enum;
#define LOSER ((role_enum) 1)
#define WINNER ((role_enum) 2)
#define CHAMPION ((role_enum) 4)
#define NOOP ((role_enum) 8)
#define DROPOUT ((role_enum) 16)
typedef unsigned char boolean;
//volatile boolean champion_sense = False;

typedef struct {
	volatile boolean *opponent;
	role_enum role;
	volatile boolean flag;
}round_t;

round_t rounds[OMP_MAX_NUM_THREADS][OMP_LOG_MAX_NUM_THREADS];

struct omp_v_thread_barr {
	volatile int parity; // any barrier thread private data
	volatile boolean sense; // any barrier thread private data
	volatile round_t *myrounds; // tournament barrier thread private data
}team[OMP_MAX_NUM_THREADS];

#endif

