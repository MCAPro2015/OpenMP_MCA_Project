/* File : omp_rtl.c
 * Contents : definition of the OpenMP RTL routine using MCA
 * Contributor : Besar Wicaksono
 *							 Cheng Wang  -- after Jan. 2012
 */


#include "omp_rtl.h"
#include "omp_util.h"
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <pthread.h>

#define PTHREADS
#define BARR_TOUR
//#define DEBUG
/* global variables used in RTL, declared in omp_thread.c */
volatile int __omp_nested = OMP_NESTED_DEFAULT;	  /* nested enable/disable */
volatile int __omp_dynamic = OMP_DYNAMIC_DEFAULT;	  /* dynamic enable/disable */
volatile int __omp_max_num_threads = OMP_MAX_NUM_THREADS;	/* max num of mca nodes (execution engine) */
volatile int __omp_log2_num_threads=0; /*store the log number of threads reqested by the users*/
volatile int __omp_nthreads_var;	/* stores the number of threads requested for future parallel regions. */
volatile unsigned long int __omp_stack_size = OMP_STACK_SIZE_DEFAULT;
volatile omp_sched_t __omp_rt_sched_type = OMP_SCHED_DEFAULT;
int __omp_rt_sched_size = OMP_CHUNK_SIZE_DEFAULT;
volatile int __omp_shmem_size = OMP_SHMEM_SIZE;
volatile int __omp_shmem_node = OMP_SHMEM_NODE;

int __omp_num_cpus;
//int is_finished = 0;
int init_rtl = 0;

/*global barrier variables*/
volatile int global_barrier_flag=0;
volatile int global_single_count = 0;
volatile boolean champion_sense = False;



__thread mrapi_shmem_hndl_t shmem_meta;
__thread shmem_data;              //commment by Cheng at 04/19/12
//__thread mrapi_shmem_hndl_t shmem_data;   //added by Cheng at 04/19/12
__thread shared_memory_slot_t* addr_meta_root=NULL;		//Cheng 05/22
__thread void* addr_data_root=NULL;  //Cheng 05/22

/*global synchronization variables -- By Cheng 05 /18*/
#ifdef MRAPI
mrapi_mutex_hndl_t mutex_barrier;
mrapi_mutex_hndl_t mutex_scheduling;
mrapi_mutex_hndl_t mutex_critical;
mrapi_mutex_hndl_t mutex_single;
mrapi_mutex_hndl_t mutex_reduction;
mrapi_mutex_hndl_t mutex_rwlck;
#endif 

#ifdef PTHREADS
pthread_mutex_t pthread_barrier;
//pthread_mutex_t pthread_barrier2;
pthread_mutex_t pthread_scheduling;
pthread_mutex_t pthread_critical;
pthread_mutex_t pthread_single;
pthread_mutex_t pthread_reduction;
#endif
int __ompc_is_ordered (omp_sched_t schedtype)
{
  switch(schedtype)
    {
    case OMP_SCHED_STATIC_EVEN:
    case OMP_SCHED_STATIC:
    case OMP_SCHED_DYNAMIC:
    case OMP_SCHED_GUIDED:
    case OMP_SCHED_RUNTIME:
    case OMP_SCHED_UNKNOWN:
      return 0;
      break;
    case OMP_SCHED_ORDERED_STATIC_EVEN:
    case OMP_SCHED_ORDERED_STATIC:
    case OMP_SCHED_ORDERED_DYNAMIC:
    case OMP_SCHED_ORDERED_GUIDED:
    case OMP_SCHED_ORDERED_RUNTIME:
      return 1;
      break;
    default:
      return 0;
    }
}

static inline int __omp_get_num_cpu()
{
	int num_cpu;
	mrapi_resource_t *root;
	mrapi_rsrc_filter_t filter;
	mrapi_resource_t *child;
	mca_status_t mrapi_status;
	
	filter = MRAPI_RSRC_CPU;
	root = mrapi_resources_get(filter, &mrapi_status);
	if (root == NULL) 
	  num_cpu = 0;
	else
	num_cpu = root->number_of_children;
	
	mrapi_resource_tree_free(&root, &mrapi_status);
    //printf("num_cpu is %d\n",num_cpu);	
	return num_cpu;
}

void print_slots()
{
	int i=0;
	shared_memory_slot_t * slot = &(addr_meta_root[0]);
    while(slot != NULL)
	{
		printf("(%d) FINISH slot->size: %d slot->feb: %d slot->offset: %p ",i,slot->size,slot->feb, slot->offset);
		if(slot->offset_prev != -1)printf("slot->prev->offset: %p ",((shared_memory_slot_t*)((unsigned long)addr_meta_root + slot->offset_prev))->offset);
		if(slot->offset_next != -1)printf("slot->next->offset: %p ",((shared_memory_slot_t*)((unsigned long)addr_meta_root + slot->offset_next))->offset);
		printf("\n");
		
		if(slot->offset_next != -1)
			slot = (shared_memory_slot_t*)((unsigned long)addr_meta_root + slot->offset_next);
		else
			slot = NULL;
		i++;
	}
}

void __ompc_parse_environment_variables()
{
	char *env_var_str;
	int  env_var_val;
	
	env_var_str = getenv("OMP_NUM_THREADS");
	if (env_var_str != NULL) {
		sscanf(env_var_str, "%d", &env_var_val);
		Is_Valid(env_var_val > 0, ("OMP_NUM_THREADS should be positive")); 
		if (env_var_val > __omp_max_num_threads)
			env_var_val = __omp_max_num_threads;
		__omp_nthreads_var = env_var_val;
	}
	
	env_var_str = getenv("OMP_SHMEM_SIZE");
	if (env_var_str != NULL) {
		sscanf(env_var_str, "%d", &env_var_val);
		Is_Valid(env_var_val > 0, ("OMP_SHMEM_SIZE should be positive")); 
		__omp_shmem_size = env_var_val;
	}
	
	env_var_str = getenv("OMP_SHMEM_NODE");
	if (env_var_str != NULL) {
		sscanf(env_var_str, "%d", &env_var_val);
		Is_Valid(env_var_val > 0, ("OMP_SHMEM_NODE should be positive"));
		__omp_shmem_node = env_var_val;
	}

	env_var_str = getenv("OMP_DYNAMIC");
	if (env_var_str != NULL) {
		env_var_val = strncasecmp(env_var_str, "true", 4);
		
		if (env_var_val == 0) {	
			__omp_dynamic = 1; 
		} else {
			env_var_val = strncasecmp(env_var_str, "false", 4);
			if (env_var_val == 0) {
				__omp_dynamic = 0;
			} else {
				Not_Valid("OMP_DYNAMIC should be set to: true/false");
			}
		}
	}

	env_var_str = getenv("OMP_NESTED");
	if (env_var_str != NULL) {
		env_var_val = strncasecmp(env_var_str, "true", 4);
		
		if (env_var_val == 0) {	
			__omp_nested = 1; 
		} else {
			env_var_val = strncasecmp(env_var_str, "false", 4);
			if (env_var_val == 0)  {
				__omp_nested = 0;
			} else {
				Not_Valid("OMP_NESTED should be set to: true/false");
			}
		}
	}
	
	env_var_str = getenv("OMP_SCHEDULE");
	if (env_var_str != NULL) {
		env_var_str = Trim_Leading_Spaces(env_var_str);
		if (strncasecmp(env_var_str, "static", 6) == 0) {
			env_var_str += 6;
			__omp_rt_sched_type = OMP_SCHED_STATIC;
		} else if (strncasecmp(env_var_str,"dynamic",7) == 0) {
			env_var_str += 7;
			__omp_rt_sched_type = OMP_SCHED_DYNAMIC;
		} else if (strncasecmp(env_var_str,"guided",6) == 0) {
			env_var_str += 6;
			__omp_rt_sched_type = OMP_SCHED_GUIDED;
		} else {
			Not_Valid("using: OMP_SCHEDULE=\"_schedule_[, _chunk_]\"");
		}

		env_var_str = Trim_Leading_Spaces(env_var_str);
		
		if (*env_var_str != '\0') {
			Is_Valid(*env_var_str == ',', 
			("a ',' is expected before the chunksize"));
			env_var_str = Trim_Leading_Spaces(++env_var_str);
			Is_Valid(isdigit((int)(*env_var_str)),
			("positive number expected for chunksize"));
			sscanf(env_var_str, "%d", &env_var_val);
			Is_Valid(env_var_val > 0, 
			("positive number expected for chunksize"));
			__omp_rt_sched_size = env_var_val;
		} else { /* no chunk size specified */
			if(__omp_rt_sched_type == OMP_SCHED_STATIC)
			__omp_rt_sched_type = OMP_SCHED_STATIC_EVEN;
		}

	}

	/* determine the stack size of slave threads. */
	env_var_str = getenv("OMP_SLAVE_STACK_SIZE");
	if (env_var_str != NULL) {
		char stack_size_unit;
		unsigned long int stack_size;
		sscanf(env_var_str, "%ld%c", &stack_size, &stack_size_unit);
		Is_Valid( stack_size > 0, ("stack size must be positive"));
		switch (stack_size_unit) {
		case 'k':
		case 'K':
			stack_size *= 1024;
			break;
		case 'm':
		case 'M':
			stack_size *= 1024 * 1024;
			break;
		case 'g':
		case 'G':
			stack_size *= 1024 * 1024 * 1024;
			break;
		default:
			Not_Valid("Using _stacksize_[kKmMgG]");
			break;
		}
		if (stack_size < OMP_STACK_SIZE_DEFAULT)
		{
			Warning("specified a slave stack size less than 4MB, using default.");
		}
		/* maybe we also need to check the upper limit? */
		Is_Valid( stack_size < Get_System_Stack_Limit(),
		("beyond current user stack limit, try using ulimit"));
		__omp_stack_size = stack_size;
	}
}

void tour_barrier_init(int vpid, volatile round_t **myrounds)
{
    int k;
	
    for(k = 1; k < __omp_nthreads_var; __omp_log2_num_threads++, k << 1);
    printf("omp_log2_num_threads = %d\n",__omp_log2_num_threads); 
    //round_t rounds[__omp_nthreads_var][__omp_log2_num_threads];
     
    (*myrounds) = &(rounds[vpid][0]);
	for(k = 0; k < OMP_LOG_MAX_NUM_THREADS; k++) 
		(*myrounds)[k].role = DROPOUT; /* default */
	
	for(k = 0; k < OMP_LOG_MAX_NUM_THREADS; k++) {
		int vpid_mod_2_sup_k_plus_1 = vpid % (1 << (k+1));
		if (vpid_mod_2_sup_k_plus_1 == 0) {
			int partner = vpid + (1 << k);
			if (partner < __omp_max_num_threads) 
				(*myrounds)[k].role = WINNER;
			else 
				(*myrounds)[k].role = NOOP;
			(*myrounds)[k].opponent = (boolean *) NULL;
		}
		else if (vpid_mod_2_sup_k_plus_1 == (1 << k)) {
			(*myrounds)[k].role = LOSER;
			(*myrounds)[k].opponent = &(rounds[vpid - (1<<k)][k].flag);
			break;
		}
	}

	if (vpid == 0) {
		(*myrounds)[k-1].role = CHAMPION;
	}

	for(k = 0; k < OMP_LOG_MAX_NUM_THREADS; k++) {
		(*myrounds)[k].flag = False;
	}
}

int setup_nodes()
{
	int i, pthread_return_value;
	omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
	mca_status_t mrapi_status;
    mrapi_key_t lock_key_rw;	
  /* Initially allocate the nodes, and vthread according to the maximum number of nodes allowed */
    
	/* allocate node data structure */
	// posix_memalign((void**)&list_nodes, CACHE_LINE_SIZE, sizeof(omp_mca_node_t)*icvs->__omp_max_num_threads);
	unsigned long offset_list_nodes = __ompc_allocate(sizeof(omp_mca_node_t)*icvs->__omp_max_num_threads);
	icvs->offset_list_nodes = offset_list_nodes;
	//printf("offset_list_nodes:%p\n",offset_list_nodes);
	omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
	//printf("setup_nodes icvs:%p icvs->offset_list_nodes:%p list_nodes:%p\n",icvs,icvs->offset_list_nodes,list_nodes);
	
	/* allocate thread team data structure */
	omp_team_t* root_team;
	// posix_memalign((void**)&root_team, CACHE_LINE_SIZE, sizeof(omp_team_t));
	unsigned long offset_root_team = __ompc_allocate(sizeof(omp_team_t));
	root_team = (omp_team_t*)((unsigned long)addr_data_root + offset_root_team);
	//printf("offset_root_team: %p\n",offset_root_team);
	
	/* allocate thread data structure */
	omp_v_thread_t* root_user_threads;
	// posix_memalign((void**)&root_user_threads, CACHE_LINE_SIZE, sizeof(omp_v_thread_t)*icvs->__omp_max_num_threads);
	unsigned long offset_root_user_threads = __ompc_allocate(sizeof(omp_v_thread_t)*icvs->__omp_max_num_threads);
	root_user_threads = (omp_v_thread_t*)((unsigned long)addr_data_root + offset_root_user_threads);
	
	/* initialize the nodes */
	// int semkey = 0xdeaddead;

	for(i=0;i<icvs->__omp_max_num_threads;i++)
	{
		list_nodes[i].domain_id = 0;
		list_nodes[i].node_id = i;
//		list_nodes[i].list_team = root_team;
        // posix_memalign((void**)&(list_nodes[i].list_team), CACHE_LINE_SIZE, sizeof(omp_team_t*)*OMP_NESTED_DEPTH);
        list_nodes[i].offset_list_team = __ompc_allocate(sizeof(unsigned long)*OMP_NESTED_DEPTH);
        unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + list_nodes[i].offset_list_team);
        offset_list_team[0] = offset_root_team;
//         icvs->list_nodes[i].list_team[0] = root_team;
//        list_nodes[i].last_team = root_team;
        // posix_memalign((void*)&(list_nodes[i].list_vthread_id), CACHE_LINE_SIZE, sizeof(int)*OMP_NESTED_DEPTH);
        list_nodes[i].offset_list_vthread_id = __ompc_allocate(sizeof(unsigned long)*OMP_NESTED_DEPTH);
        list_nodes[i].new_work = 0;
		list_nodes[i].num_team = 1;
		list_nodes[i].offset_stack_pointer = __ompc_allocate(icvs->__omp_stack_size);
        list_nodes[i].omp_exe_mode = OMP_EXE_MODE_SLEEP;
        list_nodes[i].create_new_node = -1;
        list_nodes[i].is_ready = 0;
		list_nodes[i].loop_count = 0;
		list_nodes[i].schedule_count = 0;
		list_nodes[i].ordered_count = 0;
	}
    list_nodes[0].omp_exe_mode = OMP_EXE_MODE_SEQUENTIAL;
    list_nodes[0].is_ready = 1;


	/* initialize the team */
	root_team->team_id = 0;
	root_team->total_team_size = icvs->__omp_max_num_threads;
    root_team->active_team_size = 1;
	root_team->team_level = 0;
	root_team->is_nested = 0;
	root_team->offset_list_vthread = offset_root_user_threads;
    root_team->entry_func = NULL;
    root_team->count_barrier = 0;
    root_team->num_param = 0;
    root_team->offset_param_index = -1;
	root_team->loop_count = 0;
	root_team->schedule_count = 0;
	root_team->ordered_count = 0;
	
	/*begin to create semaphore and mutex locks*/
	
#ifdef MRAPI 
  //mrapi_mutex_hndl_t mutex_barrier = mrapi_mutex_create(OMP_MCA_MUT_KEY_BARRIER_OFFSET+root_team->team_id,NULL /*atttrs */,&mrapi_status);
  mutex_barrier = mrapi_mutex_create(OMP_MCA_MUT_KEY_BARRIER_OFFSET, NULL /*atttrs */,&mrapi_status); //By Cheng 05/18

#ifdef DEBUG
	if(mrapi_status != MRAPI_SUCCESS)
	  printf("Faild to create mutex_barrier\n");
#endif
 
   mutex_critical = mrapi_mutex_create(OMP_MCA_MUT_KEY_CRITICAL_OFFSET, NULL, &mrapi_status);
#ifdef DEBUG
	if(mrapi_status != MRAPI_SUCCESS)
	  printf("Failed to create mutex_lock\n");
#endif
    

  //mrapi_mutex_hndl_t mutex_scheduling = mrapi_mutex_create(OMP_MCA_MUT_KEY_SCHEDULING_OFFSET+root_team->team_id,NULL /*atttrs */,&mrapi_status);
  //mrapi_mutex_hndl_t mutex_scheduling = mrapi_mutex_create(OMP_MCA_MUT_KEY_SCHEDULING_OFFSET,NULL /*atttrs */,&mrapi_status);
  mutex_scheduling = mrapi_mutex_create(OMP_MCA_MUT_KEY_SCHEDULING_OFFSET,NULL /*atttrs */,&mrapi_status);

#ifdef DEBUG 
	if(mrapi_status != MRAPI_SUCCESS)
	  printf("Failed to create mutex_scheduling\n");
#endif  
   
    mutex_single = mrapi_mutex_create(OMP_MCA_MUT_KEY_SINGLE_OFFSET, NULL, &mrapi_status);
#ifdef DEBUG
	if(mrapi_status != MRAPI_SUCCESS)
	  printf("Failed to create mutex_single_lock\n");
#endif
    
    mutex_reduction = mrapi_mutex_create(OMP_MCA_MUT_KEY_REDUCTION_OFFSET, NULL, &mrapi_status);
#ifdef DEBUG
	if(mrapi_status != MRAPI_SUCCESS)
	  printf("Failed to create mutex_reduction_lock\n");
#endif
    mutex_rwlck = mrapi_mutex_create(OMP_MCA_MUT_KEY_RWLCK_OFFSET, NULL, &mrapi_status);
#ifdef DEBUG
	if(mrapi_status != MRAPI_SUCCESS)
	  printf("Failed to create mutex_rw_lock\n");
#endif

#endif

/*    //Cheng 05/22
	for(i=1;i<icvs->__omp_max_num_threads;i++){
			mrapi_sem_lock(turnstile,MRAPI_TIMEOUT_INFINITE, &mrapi_status);
#ifdef DEBUG
			if(mrapi_status == MRAPI_SUCCESS)
				printf("success to init semaphore locks\n");
#endif
	}
*/
    // By Cheng 08/01/2012 -- temp. change to pthread sync. pritmitives, just
    // for performance eval. purpose
#ifdef PTHREADS
    pthread_mutex_init(&pthread_barrier,NULL);
    //pthread_mutex_init(&pthread_barrier2,NULL);
    pthread_mutex_init(&pthread_scheduling,NULL);
    pthread_mutex_init(&pthread_critical,NULL);
    pthread_mutex_init(&pthread_single,NULL);
    pthread_mutex_init(&pthread_reduction,NULL);
#endif
	/* initialize the user threads */
	for(i=0;i<icvs->__omp_max_num_threads;i++)
	{
		root_user_threads[i].node_id = i;
		root_user_threads[i].vthread_id = i;
		int* vthread_id = (int*)((unsigned long)addr_data_root + list_nodes[i].offset_list_vthread_id);
		vthread_id[0] = i;
	}

    /*tournament data initialziation*/
	 //for(i=0;i<icvs->__omp_nthreads_var;i++)
	 for(i=0;i<icvs->__omp_max_num_threads;i++)
     {
		tour_barrier_init(i,&(team[i].myrounds));
		team[i].parity = 0;
		team[i].sense = True;
	 }

    /* give order to the newly created mca node to create another node*/
    for (i=1; i<(icvs->__omp_max_num_threads-1); i++) {
        list_nodes[i].create_new_node = i+1; /* contains the node_id for the new node */
    }
    
    //printf("setup_nodes icvs->offset_list_nodes:%p\n",icvs->offset_list_nodes);
    __ompc_create_mca_node(list_nodes[1].domain_id,list_nodes[1].node_id);
    /* wait until all threads are ready */
    int last_node_id = icvs->__omp_max_num_threads-1;
    while (list_nodes[last_node_id].is_ready != 1) {
           //usleep(OMP_NEW_NODE_SLEEP);
           //usleep(1000);
	        //mrapi_mutex_lock(mutex_rwlck, &lock_key_rw, MRAPI_TIMEOUT_INFINITE, &mrapi_status);  //Cheng 05/22
	        //mrapi_mutex_unlock(mutex_rwlck, &lock_key_rw, &mrapi_status);  //Cheng 05/22
          //printf("is ready = %d\n",list_nodes[last_node_id].is_ready);
    }
   /* while(1)
    {
      if(list_nodes[last_node_id].is_ready ==1 )
        break;
    }
    */
    //printf("is ready = %u\n",list_nodes[last_node_id].is_ready);

    //printf("All nodes are ready\n");
}


void setup_shmem()
{
	mca_status_t mrapi_status;
	int i;
	shmem_meta = mrapi_shmem_create(OMP_SHMEM_META_KEY,sizeof(shared_memory_slot_t) * __omp_shmem_node,NULL,0,NULL,0,&mrapi_status);
#ifdef DEBUG
    if(mrapi_status == MRAPI_SUCCESS)
    {
      printf("success to create shmem_meta\n"); 
    } 
#endif

	if (mrapi_status == MRAPI_ERR_SHM_EXISTS) {
		printf("shmem_meta exists, try getting it from the other node\n");
		shmem_meta = mrapi_shmem_get(OMP_SHMEM_META_KEY,&mrapi_status);
	}
	
	addr_meta_root = NULL;
	addr_meta_root = mrapi_shmem_attach(shmem_meta,&mrapi_status);
	
#ifdef DEBUG
    if (mrapi_status != MRAPI_SUCCESS) { 
		printf("failed to attach shmem meta\n");
	}
#endif
	shmem_data = mrapi_shmem_create(OMP_SHMEM_DATA_KEY,__omp_shmem_size ,NULL,0,NULL,0,&mrapi_status); 
	//shmem_data = mrapi_shmem_create(OMP_SHMEM_DATA_KEY,__omp_shmem_size * 1024,NULL,0,NULL,0,&mrapi_status); //by Cheng - 05/14 shmem size is 512x1024


#ifdef DEBUG
    if(mrapi_status == MRAPI_SUCCESS)
    {
      printf("success to create shmem_data\n");
    }
#endif

	if (mrapi_status == MRAPI_ERR_SHM_EXISTS) {
		printf("shmem_data exists, try getting it from the other node\n");
		shmem_data = mrapi_shmem_get(OMP_SHMEM_DATA_KEY,&mrapi_status);
	}
	
	addr_data_root = mrapi_shmem_attach(shmem_data,&mrapi_status);
#ifdef DEBUG
    if (mrapi_status != MRAPI_SUCCESS) { 
		printf("failed to attach shmem data\n");
	}
#endif
/*
#ifdef DEBUG
    if(mrapi_status == MRAPI_SUCCESS){
      printf("success to attach shmem data\n");
    }
#endif
*/	
	// addr_meta_root[0].addr = addr_data_root;
	addr_meta_root[0].offset = 0;
	addr_meta_root[0].size = sizeof(omp_icv_t);
	addr_meta_root[0].feb = 1;
	addr_meta_root[0].available = 1;
// 	addr_meta_root[0].next = &(addr_meta_root[1]);
// 	addr_meta_root[0].prev = NULL;
	addr_meta_root[0].offset_next = sizeof(shared_memory_slot_t);
	addr_meta_root[0].offset_prev = -1;
	
	// addr_meta_root[1].addr = addr_data_root + sizeof(omp_icv_t);
	addr_meta_root[1].offset = sizeof(omp_icv_t);
	addr_meta_root[1].size = (__omp_shmem_size ) - sizeof(omp_icv_t);
	//addr_meta_root[1].size = (__omp_shmem_size * 1024) - sizeof(omp_icv_t);  //change by Cheng 05/15 change the size to 512x 1024
	addr_meta_root[1].feb = 0;
	addr_meta_root[1].available = 1;
//  addr_meta_root[1].next = NULL;
// 	addr_meta_root[1].prev = &(addr_meta_root[0]);
	addr_meta_root[1].offset_next = -1;
	addr_meta_root[1].offset_prev = 0;
	
	for(i=2;i<__omp_shmem_node;i++)
	{
		// addr_meta_root[i].addr = 0;
		addr_meta_root[i].offset = -1;
		addr_meta_root[i].size = 0;
		addr_meta_root[i].feb = 0;
		addr_meta_root[i].available = 0;
		addr_meta_root[i].offset_next = -1;
		addr_meta_root[i].offset_prev = -1;
	}
	
	omp_icv_t* list_icv = (omp_icv_t*)addr_data_root;
	list_icv->__omp_nested = __omp_nested;
	list_icv->__omp_dynamic = __omp_dynamic;
	list_icv->__omp_max_num_threads = __omp_max_num_threads;
	list_icv->__omp_nthreads_var = __omp_nthreads_var;
	list_icv->__omp_stack_size = __omp_stack_size;
	list_icv->__omp_shmem_size = __omp_shmem_size;
	list_icv->__omp_shmem_node = __omp_shmem_node;
	list_icv->__omp_num_cpus = __omp_num_cpus;
	//list_icv->is_finished = is_finished;
	list_icv->is_finished = 0;
    
}

unsigned long __ompc_allocate(size_t size)
{
	shared_memory_slot_t * slot = &(addr_meta_root[0]);
	omp_icv_t* icvs = (omp_icv_t*)addr_data_root; 	
	// printf("addr_data_root: %p icvs: %p slot:%p\n",addr_data_root,icvs,slot);
	while(slot != NULL)
	{
	 	// printf("slot: %p offset: %p offset_next:%p\n",slot,slot->offset,slot->offset_next);
		if(slot->feb == 0 && slot->size >= size)
			break;
		else
		{
			if(slot->offset_next != -1)
			{
				slot = (shared_memory_slot_t*)((unsigned long)addr_meta_root + (unsigned long)slot->offset_next);
				// printf("slot_next: %p\n",slot);
			}
			else 
				slot = NULL;
		}
	}
	
	// printf("__ompc_allocate size: %d slot: %p\n",size,slot);
	if(slot != NULL)
	{
		
		if(slot->size == size)
		{
			slot->feb = 1;
			// return slot->addr;
			// printf("return slot->offset: %p\n",slot->offset);
			return slot->offset;
		}
		else
		{
			int i;
			shared_memory_slot_t * new_slot = NULL;
			/* split node, search for unused slot */
			for (i = 0; i < icvs->__omp_shmem_node; i++) {
				// printf("addr_meta_root[%d].available = %d\n",i,addr_meta_root[i].available);
				if (addr_meta_root[i].available == 0) {
					new_slot = &(addr_meta_root[i]);
					break;
				}
			}
			
			// printf("new_slot: %p\n",new_slot);
			if(new_slot != NULL)
			{
				/* update a meta node */
				// new_slot->addr = slot->addr + size;
				new_slot->offset = slot->offset + size;
				new_slot->size = slot->size - size;
				new_slot->feb = 0;
				new_slot->available = 1;
				new_slot->offset_next = slot->offset_next;
				new_slot->offset_prev = (unsigned long)slot - (unsigned long)addr_meta_root;
				
				slot->size = size;
				slot->feb = 1;
				slot->offset_next = (unsigned long)new_slot - (unsigned long)addr_meta_root;
			}
			else
			{
				printf("ran out of shmem meta slot\n");
				slot->feb = 1;
//				exit(1);
			}
 			// printf("return slot->offset: %p\n",slot->offset);
			// return slot->addr;
			return slot->offset;
		}
 		// printf("return slot->offset: %p\n",slot->offset);
		// return slot->addr;
		return slot->offset;
	}
	else
		return -1;
		// return NULL;
}

void __ompc_deallocate(void* p)
{
	shared_memory_slot_t * slot = &(addr_meta_root[0]);
	while(slot != NULL)
	{
// 		printf("__ompc_deallocate: %p slot->feb: %d slot->addr: %p ",p,slot->feb, slot->addr);
// 		if(slot->prev != NULL)printf("slot->prev->addr: %p ",slot->prev->addr);
// 		if(slot->next != NULL)printf("slot->next->addr: %p ",slot->next->addr);
// 		printf("\n");
		if(slot->feb == 1 && (unsigned long)((unsigned long)addr_data_root + (unsigned long)slot->offset) == (unsigned long)p)
			break;
		else
		{
			if(slot->offset_next != -1)
				slot = (shared_memory_slot_t*)((unsigned long)addr_meta_root + (unsigned long)slot->offset_next);
			else
				slot = NULL;
		}
	}
	
	if(slot != NULL)
	{
		/* mark it as empty */
		slot->feb = 0;
		
		/* find slot to merge */
		if(slot->offset_prev != -1 && ((shared_memory_slot_t*)((unsigned long)addr_meta_root + slot->offset_prev))->feb == 0)
		{
			/* merge with prev node */
			// slot->prev->size = slot->prev->size + slot->size;
			shared_memory_slot_t* slot_prev = (shared_memory_slot_t*)((unsigned long)addr_meta_root + (unsigned long)slot->offset_prev);
			slot_prev->size = slot_prev->size + slot->size;
			// slot->prev->next = slot->next;
			slot_prev->offset_next = slot->offset_next;

			if(slot->offset_next != -1)
			{
				shared_memory_slot_t* slot_next = (shared_memory_slot_t*)((unsigned long)addr_meta_root + (unsigned long)slot->offset_next);
				if(slot_next->feb == 0)
				{
					/* merge also with the next node */
					// slot->prev->size = slot->prev->size + slot->next->size;
					slot_prev->size = slot_prev->size + slot_next->size;
					// slot->prev->next = slot->next->next;
					slot_prev->offset_next = slot_next->offset_next;
					
					if(slot_next->offset_next != -1)
					{
						shared_memory_slot_t* slot_next_next = (shared_memory_slot_t*)((unsigned long)addr_meta_root + slot_next->offset_next);
						slot_next_next->offset_prev = slot->offset_prev;
						// slot->next->next->prev = slot->prev;
					}
					/* mark the next slot as unavailable */
					// slot->next->addr = 0;
					slot_next->offset = -1;
					slot_next->size = 0;
					slot_next->feb = 0;
					slot_next->available = 0;
					slot_next->offset_next = -1;
					slot_next->offset_prev = -1;
				}
				else
					slot_next->offset_prev = slot->offset_prev;
			}
			
			/* mark the deallocated slot as unvailable */
			// slot->addr = 0;
			slot->offset = -1;
			slot->size = 0;
			slot->feb = 0;
			slot->available = 0;
			slot->offset_next = -1;
			slot->offset_prev = -1;
		}
		else if(slot->offset_next != -1 && ((shared_memory_slot_t*)((unsigned long)addr_meta_root + slot->offset_next))->feb == 0)
		{
			/* merge with next slot */
			shared_memory_slot_t * slot_temp = (shared_memory_slot_t*)((unsigned long)addr_meta_root + (unsigned long)slot->offset_next);
			slot->size = slot_temp->size + slot->size;
			// slot->next = slot->next->next;
			slot->offset_next = slot_temp->offset_next;
			slot->feb = 0;
			
// 			if(slot_temp->next != NULL)
// 				slot_temp->next->prev = slot;
			if(slot_temp->offset_next != -1)
			{
				shared_memory_slot_t* slot_temp_next = (shared_memory_slot_t*)((unsigned long)addr_meta_root + (unsigned long)slot_temp->offset_next);
				slot_temp_next->offset_prev = (unsigned long)slot - (unsigned long)addr_meta_root;
			}
			
			/* mark the next slot as unavailable */
			// slot_temp->addr = 0;
			slot_temp->offset = -1;
			slot_temp->size = 0;
			slot_temp->feb = 0;
			slot_temp->available = 0;
			slot_temp->offset_next = -1;
			slot_temp->offset_prev = -1;
		}
		
	}
	else
	{
		//printf("can't find slot with offset: %p\n",p);
	}
}

void __ompc_deallocate_offset(unsigned long offset)
{
	__ompc_deallocate((void*)((unsigned long)addr_data_root + offset));
}

void __ompc_setup_param_offset(int num_param)
{
	mca_status_t mrapi_status;
	
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
	omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    omp_mca_node_t* n = &(list_nodes[mrapi_node_id]);
    
    unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n->offset_list_team);
    omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n->num_team - 1]);
    
    last_team->num_param = num_param;
    last_team->offset_param_index = __ompc_allocate(num_param*sizeof(void*));
    // printf("setup offset_param_index : %p\n",last_team->offset_param_index);
}

void* __ompc_get_param_offset(int idx)
{
	mca_status_t mrapi_status;
	
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    omp_mca_node_t* n = &(list_nodes[mrapi_node_id]);
    
    unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n->offset_list_team);
    omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n->num_team - 1]);
    
    if(idx >= 0 && idx < last_team->num_param)
    {
    	void** param_index = (void**)((unsigned long)addr_data_root + last_team->offset_param_index);
    	// printf("get offset_param_index : %p param_index : %p value : %p\n",last_team->offset_param_index,param_index,param_index[idx]);
    	return (void*)((unsigned long)param_index[idx]);
    }
    else 
    	return NULL;
}

void __ompc_fill_param_offset(int idx, void* addr)
{
	mca_status_t mrapi_status;
	
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    omp_mca_node_t* n = &(list_nodes[mrapi_node_id]);
    
    unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n->offset_list_team);
    omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n->num_team - 1]);
    
    void** param_index = (void**)((unsigned long)addr_data_root + last_team->offset_param_index);
    param_index[idx] = addr;
    // printf("fill offset_param_index : %p param_index : %p value : %p\n",last_team->offset_param_index,param_index,param_index[idx]);
}

int __ompc_create_mca_node(int domain_id,int node_id)
{
    int pthread_return_value;
    pthread_attr_t __omp_pthread_attr;
    
    //pthread_mutex_init(&p_lock1, NULL);  //Cheng 05/21
    //pthread_mutex_init(&p_lock2,NULL);
    
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    //printf("node_id:%d addr_data_root:%p icvs->offset_list_nodes:%p\n",node_id,addr_data_root,icvs->offset_list_nodes);
    
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    //printf("node_id:%d list_nodes:%p\n",node_id,list_nodes);
    
    pthread_attr_init(&__omp_pthread_attr);
	  pthread_attr_setscope(&__omp_pthread_attr, PTHREAD_SCOPE_SYSTEM);
    char* stack_pointer = (char*)((unsigned long)addr_data_root + list_nodes[node_id].offset_stack_pointer);
    pthread_return_value = pthread_attr_setstack(&__omp_pthread_attr, stack_pointer, icvs->__omp_stack_size); 
    Is_True(pthread_return_value == 0, ("Cannot set stack pointer for thread"));
    
    pthread_return_value = pthread_create( &(list_nodes[node_id].pthread_handle),
                                          &__omp_pthread_attr, (pthread_entry)__ompc_slave_routine, 
                                  (void *)(unsigned long)node_id);
    if (pthread_return_value){
      printf("ERROR; return code from pthread_create() is %d\n", pthread_return_value);
      exit(-1);
    }
    Is_True(pthread_return_value == 0, ("Cannot create more pthreads"));
    
}

void *__ompc_slave_routine(void *node)
{
	/* mrapi init for the child node */
	mca_status_t mrapi_status;
	mrapi_parameters_t parms = 0;
	mrapi_info_t version;
	init_rtl = 1;
	unsigned long node_id = (unsigned long)(node);
    unsigned long  count=0;
    mrapi_key_t lock_key_rw;
    //printf("Node-%d is starting\n",node_id);
	
    //pthread_mutex_lock (&p_lock1);		//Cheng 05/21
	mrapi_initialize(0,node_id,parms,&version,&mrapi_status);
	//pthread_mutex_unlock (&p_lock1);
	
	if (mrapi_status  != MRAPI_SUCCESS){
		init_rtl = -1;
		printf("Child node-%d failed on mrapi_initialize: %d\n",node_id,mrapi_status);
		return NULL;
	}
	
	 
	shmem_meta = mrapi_shmem_get(OMP_SHMEM_META_KEY,&mrapi_status);
	addr_meta_root = mrapi_shmem_attach(shmem_meta,&mrapi_status);
#ifdef DEBUG
	if (mrapi_status != MRAPI_SUCCESS) { 
		printf("failed to attach shmem meta\n");
	}
#endif


    shmem_data = mrapi_shmem_get(OMP_SHMEM_DATA_KEY,&mrapi_status);
	addr_data_root = mrapi_shmem_attach(shmem_data,&mrapi_status);
#ifdef DEBUG	
    if (mrapi_status != MRAPI_SUCCESS) { 
		printf("failed to attach shmem data\n");
	}
#endif

	omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
	//printf("slave_routine TID-%d icvs:%p icvs->offset_list_nodes\n",node_id,icvs);
	omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
	
	omp_mca_node_t* n = &(list_nodes[node_id]);  // n is for each node id now --Cheng
	
	unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n->offset_list_team);
    
    //printf("node id is %d\n", n->node_id); 
    n->is_ready = 1;
    
    if (n->create_new_node != -1) {
        omp_mca_node_t* n_new = &(list_nodes[n->create_new_node]);
        __ompc_create_mca_node(n_new->domain_id,n_new->node_id);
    }
    
    while(/*icvs != NULL && */icvs->is_finished == 0) // -Cheng -07/17
    {
        /* waiting for work 
         * we use busy wait since mrapi doesn't provide pthread_wait like routine
         */
        
        //printf("n->new_work=%d\n",n->new_work);
        while(/*icvs != NULL && */n->new_work == 0)
        {
            //usleep(OMP_NEW_WORK_SLEEP); /* sleep for 1000ms */
            //printf("n->new_work=%d\n",n->new_work);
	        //mrapi_mutex_lock(mutex_rwlck, &lock_key_rw, MRAPI_TIMEOUT_INFINITE, &mrapi_status);  //Cheng 05/22
	        //mrapi_mutex_unlock(mutex_rwlck, &lock_key_rw, &mrapi_status);  //Cheng 05/22
            //count++;
        }
        //printf("icvs->is_finished=%d\n",icvs->is_finished);
        //printf("n->new_work=%d\n",n->new_work);   
        
        omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n->num_team - 1]);
        // printf("slave last_team : %p\n",last_team);
        if(last_team->entry_func != NULL)
        {
            /* do something */
//            int vthread_id = node_id - n->last_team->list_vthread[0].node_id;
			int* list_vthread_id = (int*)((unsigned long)addr_data_root + n->offset_list_vthread_id);
        	int vthread_id = list_vthread_id[n->num_team - 1];
            last_team->entry_func(vthread_id, last_team->frame_pointer);
//            printf("Node-%d vthread-%d is working\n",node_id,vthread_id);
            
            /* finish */
            n->new_work = 0;
            
            //printf("FROM SLAVE ROUNTIE %d\n",vthread_id);
            /* barrier */
            __ompc_level_1_barrier(vthread_id);
            //printf("BACK from SLAVE %d\n",vthread_id);
            /* back to sleep */
            n->omp_exe_mode = OMP_EXE_MODE_SLEEP;
        }
    }

    //printf("is_finished=%d",icvs->is_finished);    
//    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
//    int test = *((int*)addr_data_root);
    // printf("__ompc_slave_routine ends for node-%d\n",node_id);
    
    // mrapi_shmem_detach(shmem_meta,&mrapi_status);
// 	mrapi_shmem_detach(shmem_data,&mrapi_status);

	//pthread_mutex_lock (&p_lock2);   //Cheng 05/22
	
	mrapi_finalize (&mrapi_status);
#ifdef DEBUG
    if (mrapi_status != MRAPI_SUCCESS)
		printf("Child node-%d failed on mrapi_finalize\n",node_id);
	//pthread_mutex_unlock (&p_lock2);	
#endif
    pthread_exit(NULL); 
	return NULL;
}

void free_node()
{
	int i;
    int rc;
	mca_status_t mrapi_status;
	
	omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
	omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
	
	mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
	omp_mca_node_t n = list_nodes[mrapi_node_id];
	
	/* wake up the child nodes so each child call the mca finalize routine */
    /* master node need to wait until the child nodes finalize their mca */
	
    for(i=1;i<icvs->__omp_max_num_threads;i++)
    {
        list_nodes[i].new_work = 1;  //--Cheng 07/16
        //printf("finishing node %d\n",i);
        pthread_join(list_nodes[i].pthread_handle,NULL);
        if (rc){
          printf("ERROR; return code from pthread_join() is %d\n", rc);
          exit(-1);
        }
        //printf("node %d is finished\n",i);
    }
	
	printf("deallocating mrapi memory\n");
    /* now only free the memory of nodes and threads in  the first team */
    unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n.offset_list_team);
	omp_team_t* root_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[0]);
	omp_v_thread_t* root_user_threads= (omp_v_thread_t*)((unsigned long)addr_data_root + root_team->offset_list_vthread);
   
   	//mrapi_sem_hndl_t turnstile = mrapi_sem_get(OMP_MCA_SEM_KEY_OFFSET,&mrapi_status);
    //mrapi_mutex_hndl_t mutex_scheduling = mrapi_mutex_get(OMP_MCA_MUT_KEY_SCHEDULING_OFFSET,&mrapi_status);
   
#ifdef PTHREADS 
    pthread_mutex_destroy(&pthread_barrier);
    //pthread_mutex_destroy(&pthread_barrier2);
    pthread_mutex_destroy(&pthread_critical);
    pthread_mutex_destroy(&pthread_single);
    pthread_mutex_destroy(&pthread_scheduling);
    pthread_mutex_destroy(&pthread_reduction);
#endif
    
    //printf("ccccccc\n");
/* 
    mrapi_sem_delete(turnstile,&mrapi_status);
    mrapi_mutex_delete(mutex_barrier,&mrapi_status);
		mrapi_mutex_delete(mutex_scheduling,&mrapi_status);
*/
    // print_slots();
    // printf("\n\n");
    for(i=0;i<icvs->__omp_max_num_threads;i++){
       	// printf("deallocating icvs->list_nodes[%d].list_team: %p\n",i,list_nodes[i].offset_list_team);
		__ompc_deallocate_offset(list_nodes[i].offset_list_team);
		// print_slots();
		// printf("\n\n");
		
		// printf("deallocating icvs->list_nodes[%d].stack_pointer: %p\n",i,list_nodes[i].offset_stack_pointer);
		__ompc_deallocate_offset(list_nodes[i].offset_stack_pointer);
		// print_slots();
		// printf("\n\n");
		
		// printf("deallocating icvs->list_nodes[%d].list_vthread_id: %p\n",i,list_nodes[i].offset_list_vthread_id);
		__ompc_deallocate_offset(list_nodes[i].offset_list_vthread_id);
		// print_slots();
		// printf("\n\n");
    }
    
    // printf("deallocating root_user_threads: %p\n",root_team->offset_list_vthread);
	__ompc_deallocate_offset(root_team->offset_list_vthread);
	// print_slots();
	// printf("\n\n");
	
	// printf("deallocating root_team: %p\n",offset_list_team[0]);
	__ompc_deallocate_offset(offset_list_team[0]);
	// print_slots();
	// printf("\n\n");
	
	// printf("deallocating icvs->list_nodes: %p\n",icvs->offset_list_nodes);
	__ompc_deallocate_offset(icvs->offset_list_nodes);
    // print_slots();
	// printf("\n\n");
	
    //print_slots();
}

int __ompc_init_rtl()
{
	//is_finished = 0;
    
	mca_status_t mrapi_status;
	mrapi_parameters_t parms = 0;
	mrapi_info_t version;
	
	mrapi_initialize(DOMAIN,NODE,parms,&version,&mrapi_status);
	if (mrapi_status  != MRAPI_SUCCESS){
		init_rtl = -1;
        printf("ERROR!Failed to initialize the mrapi\n");
		return init_rtl;
	}
    
    if (mrapi_status == MRAPI_SUCCESS)
    {
      printf("Success to initialize the mrapi\n");
    }
	
    /* get the number of hardware processors */
	__omp_num_cpus = __omp_get_num_cpu();
	if(__omp_num_cpus == 0){
		init_rtl = -2;
		return init_rtl;
	}
	
	/* number of MCA nodes */
	__omp_nthreads_var = __omp_num_cpus;

	/* parse OpenMP environment variables */
	__ompc_parse_environment_variables();
    
    //printf("num of cpus is %d\n",__omp_nthreads_var);    
	
	
	/* register the finalize function*/
	atexit(__ompc_fini_rtl);
    
    setup_shmem();
    setup_nodes();
	return init_rtl;
}

void __ompc_fini_rtl()
{
    //printf("__ompc_fini_rtl\n");
	omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
	//mrapi_key_t lock_key_rw;
    mca_status_t mrapi_status;
/*	mrapi_mutex_lock(mutex_rwlck, &lock_key_rw, MRAPI_TIMEOUT_INFINITE, &mrapi_status);  //Cheng 05/22
#ifdef DEBUG
        if(mrapi_status!=MRAPI_SUCCESS)
          printf("failed to lock mutex\n");
#endif 
*/
    icvs->is_finished = 1;
/*    
	mrapi_mutex_unlock(mutex_rwlck,&lock_key_rw, &mrapi_status);  //Cheng 05/22
#ifdef DEBUG
        if(mrapi_status!=MRAPI_SUCCESS)
          printf("failed to lock mutex\n");
#endif 
*/
    //printf("icvs->is_finish --- from ompc_fini_rtl == %d\n",icvs->is_finished);
    //int i = 0;
    
		
	free_node();
	
//	mrapi_shmem_detach(shmem_meta,&mrapi_status);
//	mrapi_shmem_detach(shmem_data,&mrapi_status);
		
	mrapi_finalize (&mrapi_status);
#ifdef DEBUG
	if (mrapi_status != MRAPI_SUCCESS)
		printf("Master node failed on mrapi_finalize\n");
#endif
}

inline int __ompc_can_fork(void)
{
    mca_status_t mrapi_status;
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t n = list_nodes[mrapi_node_id];
    
	if (n.omp_exe_mode == OMP_EXE_MODE_SEQUENTIAL)
		return 1;
	else if (n.omp_exe_mode == OMP_EXE_MODE_NORMAL && icvs->__omp_nested == 1)
		return 0;
	else if (n.omp_exe_mode == OMP_EXE_MODE_NESTED )
		return 0;
	else return 0;
}

inline int __ompc_get_local_thread_num(void)
{
    mca_status_t mrapi_status;
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t n = list_nodes[mrapi_node_id];
    
    if (n.omp_exe_mode == OMP_EXE_MODE_SEQUENTIAL) {
        return 0;
    } else {
//        int vthread_id = n.node_id - n.last_team->list_vthread[0].node_id;
    	/* return the thread id in the last team */
    	int* list_vthread_id = (int*)((unsigned long)addr_data_root + n.offset_list_vthread_id);
    	int vthread_id = list_vthread_id[n.num_team - 1];
        return vthread_id;
    }
}

void __ompc_serialized_parallel (int vthread_id)
{
    
}

void __ompc_end_serialized_parallel (int vthread_id)
{
    
}

/* required by compiler */
void __ompc_task_exit()
{

}

void __ompc_fork(const int _num_threads, omp_micro micro_task,frame_pointer_t frame_pointer)
{
    int num_threads = _num_threads;
    int i;
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    
    mca_status_t mrapi_status;
    
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t* n = &(list_nodes[mrapi_node_id]);  //n is the master node
    
    if(n->omp_exe_mode == OMP_EXE_MODE_SEQUENTIAL){
        
		/*wait until the implicit barrier is finished*/
		// USE THIS FOR COUNTING LAST TEAM, LIST_VTHREAD_ID, LIST_VTHREAD
		unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n->offset_list_team);
		omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n->num_team - 1]);
    	
        //mrapi_key_t lock_key_barrier;
		//mrapi_mutex_lock(mutex_barrier,&(lock_key_barrier),MRAPI_TIMEOUT_INFINITE, &mrapi_status);		
		//mrapi_mutex_unlock(mutex_barrier,&(lock_key_barrier),&mrapi_status);


        if (num_threads != 0) 
            num_threads = __ompc_check_num_threads(num_threads);
        
        /* Adjust the number of the number of vthread in the team */
        if (num_threads == 0) {
            /* use default vthread number decided from processor number and environment variable */
            num_threads = icvs->__omp_nthreads_var;
        }
       
        //printf("num of threads is %d\n",num_threads); 
        /* setup work for the team */
        last_team->active_team_size = num_threads;
        last_team->frame_pointer = frame_pointer;
		last_team->loop_count = 0;
		last_team->schedule_count = 0;
		last_team->ordered_count = 0;
        last_team->entry_func = micro_task;
        
        n->omp_exe_mode = OMP_EXE_MODE_NORMAL;
        
        /* wake up the child nodes if needed */
        if(last_team->active_team_size > 1){
            for(i=1;i<last_team->active_team_size;i++){
            	omp_v_thread_t* list_vthread = (omp_v_thread_t*)((unsigned long)addr_data_root + last_team->offset_list_vthread);
                list_nodes[list_vthread[i].node_id].omp_exe_mode = OMP_EXE_MODE_NORMAL;
				list_nodes[list_vthread[i].node_id].loop_count = 0;
				list_nodes[list_vthread[i].node_id].schedule_count = 0;
				list_nodes[list_vthread[i].node_id].ordered_count = 0;
               //printf("wake up %d\n",list_nodes[list_vthread[i].node_id]);
                list_nodes[list_vthread[i].node_id].new_work = 1;
            }
        }
        
        /*
        if(last_team->active_team_size > 1){
            for(i=1;i<last_team->active_team_size;i++){
            	//omp_v_thread_t* list_vthread = (omp_v_thread_t*)((unsigned long)addr_data_root + last_team->offset_list_vthread);
                list_nodes[i].omp_exe_mode = OMP_EXE_MODE_NORMAL;
				list_nodes[i].loop_count = 0;
				list_nodes[i].schedule_count = 0;
				list_nodes[i].ordered_count = 0;
               //printf("wake up %d\n",list_nodes[list_vthread[i].node_id]);
                list_nodes[i].new_work = 1;
            }
        }
        */
        /* master node starts the work */
				//printf("Node-%d vthread-%d is working\n",n->node_id,0);
        last_team->entry_func(0, last_team->frame_pointer);
        //printf("n->new_work=%d\n",n->new_work);

        /* finish */
        n->new_work = 0;
        
        //printf("Master node id is %d\n",n->node_id);
        /* barrier */
        __ompc_level_1_barrier(0);
        //printf("After the master node\n"); 
        if(last_team->num_param > 0)
        {
        	/* delete the parameter */
        	 printf("last_team->offset_param_index : %p\n",last_team->offset_param_index);
        	__ompc_deallocate_offset(last_team->offset_param_index);
        	last_team->num_param = 0;
        }
        last_team->entry_func = NULL;
        
        /* back to serial execution */
        n->omp_exe_mode = OMP_EXE_MODE_SEQUENTIAL;
    }

}
inline void __ompc_barrier(void)
{
	mca_status_t mrapi_status;
	
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t* n = &(list_nodes[mrapi_node_id]);
    
    unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n->offset_list_team);
	omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n->num_team - 1]);
    
    int* list_vthread_id = (int*)((unsigned long)addr_data_root + n->offset_list_vthread_id);
    int vthread_id = list_vthread_id[n->num_team - 1];
    
    if(n->omp_exe_mode == OMP_EXE_MODE_SEQUENTIAL)
    	return;
    else if(n->omp_exe_mode == OMP_EXE_MODE_NORMAL)
    {
    	__ompc_level_1_barrier(vthread_id);
    	return;
    }
    else
    	return;
}



inline void __ompc_ebarrier(void)
{
	mca_status_t mrapi_status;
	
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t* n = &(list_nodes[mrapi_node_id]);
    
    unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n->offset_list_team);
	omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n->num_team - 1]);
    
    int* list_vthread_id = (int*)((unsigned long)addr_data_root + n->offset_list_vthread_id);
    int vthread_id = list_vthread_id[n->num_team - 1];
    
    if(n->omp_exe_mode == OMP_EXE_MODE_SEQUENTIAL)
    	return;
    else if(n->omp_exe_mode == OMP_EXE_MODE_NORMAL)
    {
    	//__ompc_level_1_barrier(vthread_id);
        __ompc_level_1_barrier(vthread_id);
    	return;
    }
    else
    	return;
}

#ifdef BARR_CENT
void __ompc_level_1_barrier(const int vthread_id)
{
	int i;
    mrapi_key_t lock_key_barrier;
    mca_status_t mrapi_status;
	//mrapi_mutex_hndl_t mutex_barrier;
    
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t* n = &(list_nodes[mrapi_node_id]);
	
	unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n->offset_list_team);
	omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n->num_team - 1]);
    
	//printf("thread_id = %d\n", vthread_id);
	if(last_team->active_team_size > 1){
		
		//printf("active_team_size=%d\n",last_team->active_team_size);
		
		
		int barrier_flag = global_barrier_flag;
#ifdef PTHREADS	
        pthread_mutex_lock(&pthread_barrier);
#endif
        
#ifdef MRAPI
        mrapi_mutex_lock(mutex_barrier,&lock_key_barrier, MRAPI_TIMEOUT_INFINITE, &mrapi_status);  //Cheng 05/22
#ifdef DEBUG
        if(mrapi_status!=MRAPI_SUCCESS)
          printf("failed to lock mutex\n");
#endif 
#endif

        last_team->count_barrier++;           //count_barrier is a global variable -- Cheng

#ifdef PTHREADS	
	    pthread_mutex_unlock(&pthread_barrier);
#endif

#ifdef MRAPI 
        mrapi_mutex_unlock(mutex_barrier,&lock_key_barrier,&mrapi_status);
#ifdef DEBUG
        if(mrapi_status!=MRAPI_SUCCESS)
          printf("failed to lock mutex\n");
#endif
#endif

		if(last_team->count_barrier == (last_team->active_team_size)){
		//printf("count_barrier=%d\n",last_team->count_barrier);	
		
//#ifdef PTHREADS	
//        pthread_mutex_lock(&pthread_barrier);
//#endif
		//printf("n.last_team->active_team_size : %d\n",last_team->active_team_size);
			last_team->count_barrier = 0;
			global_barrier_flag = barrier_flag^1;


//#ifdef PTHREADS	
//	    pthread_mutex_unlock(&pthread_barrier);
//#endif
            //printf("barrier_flag= %d, global_barrier_flag=%d\n", barrier_flag, global_barrier_flag);
		}
		else{
			while(barrier_flag==global_barrier_flag) {};
		}
	}
}

#endif

#ifdef BARR_TOUR

/*****************************************/
/** Tournament barrier implementation**  */
/*****************************************/
void __ompc_level_1_barrier(const int vthread_id)
{
	int i=0;
    mrapi_key_t lock_key_barrier;
    mca_status_t mrapi_status;
    
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t* n = &(list_nodes[mrapi_node_id]);
	
	unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n->offset_list_team);
	omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n->num_team - 1]);
    
	//printf("thread_id = %d\n", vthread_id);
	if(last_team->active_team_size == 1)
      return;
      
    struct omp_v_thread_barr *current_thd = &team[vthread_id];
	round_t* round=current_thd->myrounds;

	for(;;) {
	  if(round->role & LOSER) {
			*(round->opponent) = current_thd->sense;
			while (champion_sense != current_thd->sense){
              //printf("loser from slave %d\n");
            }
			break;
	  }
	  else if (round->role & WINNER)
		  while (round->flag != current_thd->sense){
            //printf("winer from slave %d\n");
          }
	  else if (round->role & CHAMPION )
	  {
		  while (round->flag != current_thd->sense){
            //printf("chapman from slave %d\n");
          }
			
		  champion_sense = current_thd->sense;
		  break;	
	  }
	  round++;
      //i++;
	} // end for
	current_thd->sense ^= True;
    //printf("****from slave %d\n", vthread_id);
}

#endif 
int __ompc_check_num_threads(const int _num_threads)
{
    int num_threads = _num_threads;
    int request_threads;
    
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    
    mca_status_t mrapi_status;
    
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t n = list_nodes[mrapi_node_id];

	unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n.offset_list_team);
	omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n.num_team - 1]);
    Is_Valid( num_threads > 0,("number of threads must be possitive!"));
    
    if (n.omp_exe_mode == OMP_EXE_MODE_SEQUENTIAL) {
        request_threads = num_threads - last_team->total_team_size;
        if (request_threads > 0 && num_threads > icvs->__omp_max_num_threads) {
            /* Exceed current limit*/
            Warning(" Exceed the thread number limit: Reduce to Max");
            num_threads = icvs->__omp_max_num_threads;
        }
    } else {
        /* Request for nest team --later-- */
        num_threads = 1;
    }
    
//    printf("given num threads : %d\n",num_threads);
    return num_threads;
}

void __ompc_static_init_4 (int global_tid, omp_sched_t schedtype,
                      int *plower, 
                      int *pupper, int *pstride, 
                      int incr, int chunk) 
{
    int my_lower, my_upper;
    int block_size;
    int team_size;
    int trip_count;
    int adjustment;
    int stride;
    mca_status_t mrapi_status;
    
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t n = list_nodes[mrapi_node_id];
    
//    printf("TID-%d exe_mode:%d\n",global_tid,n.omp_exe_mode);
    if (n.omp_exe_mode == OMP_EXE_MODE_SEQUENTIAL) {
        /* in sequential part*/
        block_size = (incr > 0) ? (*pupper - *plower + 1):(*pupper - *plower - 1);
        /* plower, pupper are not changed*/
        *pstride = block_size;
        //*plastiter = 1;
        return;
    } 
    
    unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n.offset_list_team);
	omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n.num_team - 1]);
    team_size = last_team->active_team_size;
//    printf("TID-%d team_size:%d\n",global_tid,team_size);
    if (team_size == 1) {
        /* single thread schdule
         * plower, pupper are not changed
         */
        block_size = (incr > 0) ? (*pupper - *plower + 1):(*pupper - *plower - 1);
        *pstride = block_size;
        return;
    }
    
    if (schedtype == OMP_SCHED_STATIC_EVEN) {
        /* calculate the number of iteration */
        int incr2 = (incr > 0)?1:-1;
        int diff = *pupper - *plower + incr2;
        int num_iter = ((diff%incr)>0)?(diff/incr)+1:(diff/incr);
        
        /* calculate the chunk for each thread */ 
        int remainder = 0;
        int my_chunk = 1;
        if (num_iter > team_size) {
            remainder = num_iter%team_size;
            my_chunk = num_iter / team_size;
        }
        
        if (global_tid < remainder) {
            block_size = (my_chunk+1) * incr;
            *plower = *plower + global_tid * block_size;
            *pupper = *plower + block_size - incr;
        }
        else{
            block_size = (my_chunk) * incr;
            *plower = *plower + global_tid * block_size + remainder * incr;
            *pupper = *plower + block_size - incr;
        }
        
//        printf("TID-%d plower:%d pupper:%d\n",global_tid,*plower,*pupper);
        return;
    } else { /* OMP_SCHED_STATIC*/
        Is_Valid( chunk > 0, ("chunk size must be a positive number"));
        
        block_size = chunk * incr;
        *pstride = block_size * team_size;
        *plower = *plower + global_tid * block_size;
        *pupper = *plower + block_size - incr;
        
//        printf("TID-%d plower:%d pupper:%d pstrid:%d\n",global_tid,*plower,*pupper,*pstride);
        return;
    }
}

void __ompc_scheduler_init_4 (int global_tid, omp_sched_t schedtype, int lower, int upper,int stride, int chunk)
{
	
    int team_size;
    mca_status_t mrapi_status;
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t* n = &(list_nodes[mrapi_node_id]);
    
    //omp_mca_node_t n = list_nodes[mrapi_node_id];
    //unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n.offset_list_team);
    //omp_team_t** list_team = (omp_team_t**)((unsigned long)addr_data_root + n->offset_list_team);
	//omp_team_t* p_team = list_team[n->num_team - 1];
    //omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n.num_team - 1]);
    
    unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n->offset_list_team);
	omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n->num_team - 1]);

	omp_team_t* p_team = &(last_team[n->num_team - 1]);
	
	if (schedtype == OMP_SCHED_RUNTIME) {
		schedtype = __omp_rt_sched_type;
		chunk = __omp_rt_sched_size;
	} else if (schedtype == OMP_SCHED_ORDERED_RUNTIME) {
		schedtype = __omp_rt_sched_type + OMP_SCHED_ORDERED_GAP;
		chunk = __omp_rt_sched_size;
	}
	if (n->omp_exe_mode == OMP_EXE_MODE_SEQUENTIAL) {
		p_team->loop_lower_bound = lower;
		p_team->loop_upper_bound = upper;
		p_team->loop_increment = stride;
		p_team->schedule_count = 0;
		return;
	}

    //printf("active_team_size=%d\n",p_team->active_team_size); 

	n->schedule_count = 0;

	
    team_size = p_team->active_team_size;
    //printf("team_size=%d\n", p_team->active_team_size);
	if (team_size == 1) {

		p_team->loop_lower_bound = lower;
		p_team->loop_upper_bound = upper;
		p_team->loop_increment = stride;
		p_team->schedule_count = 0;
		
		if (__ompc_is_ordered(schedtype))
			p_team->ordered_count = 0;
		return;
	}
	n->loop_count++;
	
	mrapi_key_t lock_key_scheduling;
	//mrapi_mutex_hndl_t mutex_scheduling = mrapi_mutex_get(OMP_MCA_MUT_KEY_SCHEDULING_OFFSET,&mrapi_status);
#ifdef MRAPI
    mrapi_mutex_lock(mutex_scheduling,&lock_key_scheduling,MRAPI_TIMEOUT_INFINITE, &mrapi_status);		
#ifdef DEBUG
    if(mrapi_status!=MRAPI_SUCCESS)
      printf("Failed to lock mutex_scheduling\n");
#endif
#endif

#ifdef PTHREADS
    pthread_mutex_lock(&pthread_scheduling);
#endif
    if (p_team->loop_count >= n->loop_count) {
		/* the initialization for lower and upper bound is finished */
#ifdef MRAPI	
      mrapi_mutex_unlock(mutex_scheduling,&(lock_key_scheduling),&mrapi_status);
#endif

#ifdef PTHREADS
      pthread_mutex_unlock(&pthread_scheduling);
#endif
		return;
	} else {
		/* The first one call schedule_init do the initialization work */
		/* Initializing */
		p_team->loop_lower_bound = lower;
		p_team->loop_upper_bound = upper;
		p_team->loop_increment = stride;
		p_team->schedule_type = schedtype;
		p_team->chunk_size = chunk;
		p_team->schedule_count = 0;
		if (__ompc_is_ordered(schedtype))
		  p_team->ordered_count = 0;
		/* Initialization finished */
		p_team->loop_count++;
#ifdef MRAPI
		mrapi_mutex_unlock(mutex_scheduling,&(lock_key_scheduling),&mrapi_status);
#endif
#ifdef PTHREADS
        pthread_mutex_unlock(&pthread_scheduling);
#endif
		return;
	}
}

int __ompc_schedule_next_4 (int global_tid, int *plower, int *pupper, int *pstride)
{
	int	team_size;
	int	trip_count;
	int	adjustment;
	int	block_size;
	int	stride;
	int	chunk;
	int	incr;
	int	my_lower, my_upper;
	int	global_lower, global_upper;
	int	my_trip, schedule_count;
	float trip_flag;
	
	mca_status_t mrapi_status;
	omp_icv_t* icvs = (omp_icv_t*)addr_data_root;  
	omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
	
	mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t* n = &(list_nodes[mrapi_node_id]);
    
    unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n->offset_list_team);
   
	omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n->num_team - 1]);

	omp_team_t* p_team = &(last_team[n->num_team - 1]);
	
	if (n->omp_exe_mode == OMP_EXE_MODE_SEQUENTIAL) {
    /*Judge whether there are more iterations*/
		if (p_team->schedule_count != 0)return 0;
		p_team->schedule_count = 1;
		*plower = p_team->loop_lower_bound;
		*pupper = p_team->loop_upper_bound;
		*pstride = p_team->loop_increment;
		return 1;
	}
	
	team_size = p_team->active_team_size;
	if (team_size == 1) {
		/* Single thread team running: for sequentialized nested team*/
		/*Judge whether there are more iterations*/
		if (p_team->schedule_count != 0)return 0;
		p_team->schedule_count = 1;
		*plower = p_team->loop_lower_bound;
		*pupper = p_team->loop_upper_bound;
		*pstride = p_team->loop_increment;
		n->ordered_count = 0;
		return 1;
	}
	
	switch (p_team->schedule_type) {
		case OMP_SCHED_DYNAMIC:
		{
			mrapi_key_t lock_key_scheduling;
			//mrapi_mutex_hndl_t mutex_scheduling = mrapi_mutex_get(OMP_MCA_MUT_KEY_SCHEDULING_OFFSET,&mrapi_status);
#ifdef MRAPI	
			mrapi_mutex_lock(mutex_scheduling,&(lock_key_scheduling),MRAPI_TIMEOUT_INFINITE, &mrapi_status);	
#endif
#ifdef PTHREADS
            pthread_mutex_lock(&pthread_scheduling);
#endif
			global_lower = p_team->loop_lower_bound;
			global_upper = p_team->loop_upper_bound;
			incr = p_team->loop_increment;
			trip_flag = (global_upper - global_lower) * 1.0 / (float)incr;
			trip_count = (global_upper - global_lower) / incr;
			
			if ( trip_flag < 0) {
#ifdef MRAPI
			  mrapi_mutex_unlock(mutex_scheduling,&(lock_key_scheduling),&mrapi_status);
#endif
#ifdef PTHREADS
              pthread_mutex_unlock(&pthread_scheduling);
#endif
              return 0;
			}

			trip_count += 1;
			my_lower = global_lower;
			chunk = p_team->chunk_size;
			my_upper = my_lower + (chunk - 1) * incr;
			p_team->loop_lower_bound = my_lower + chunk * incr;
#ifdef MRAPI
			mrapi_mutex_unlock(mutex_scheduling,&(lock_key_scheduling),&mrapi_status);
#endif

#ifdef PTHREADS
            pthread_mutex_unlock(&pthread_scheduling);
#endif
			*plower = my_lower;
			*pupper = my_upper;
			*pstride = incr;
			
			return 1;
			break;
		}
		case OMP_SCHED_ORDERED_DYNAMIC:
		{
			mrapi_key_t lock_key_scheduling;
			//mrapi_mutex_hndl_t mutex_scheduling = mrapi_mutex_get(OMP_MCA_MUT_KEY_SCHEDULING_OFFSET,&mrapi_status);
			
#ifdef MRAPI
			mrapi_mutex_lock(mutex_scheduling,&(lock_key_scheduling),MRAPI_TIMEOUT_INFINITE, &mrapi_status);
#endif
#ifdef PTHREADS
            pthread_mutex_lock(&pthread_scheduling);
#endif
			global_lower = p_team->loop_lower_bound;
			global_upper = p_team->loop_upper_bound;
			incr = p_team->loop_increment;
			schedule_count = p_team->schedule_count;
			trip_flag = (global_upper - global_lower) * 1.0 / (float)incr;

			if ( trip_flag < 0) {
#ifdef MRAPI
			  mrapi_mutex_unlock(mutex_scheduling,&(lock_key_scheduling),&mrapi_status);
#endif
#ifdef PTHREADS
              pthread_mutex_unlock(&pthread_scheduling);
#endif
              return 0;
			}
			my_lower = global_lower;
			chunk = p_team->chunk_size;

			p_team->loop_lower_bound = my_lower + chunk * incr;
			p_team->schedule_count++;
					
#ifdef MRAPI
			mrapi_mutex_unlock(mutex_scheduling,&(lock_key_scheduling),&mrapi_status);
#endif
#ifdef PTHREADS
            pthread_mutex_unlock(&pthread_scheduling);
#endif
			trip_count = (global_upper - global_lower) / incr + 1;
			my_upper = my_lower + (chunk - 1) * incr;

			n->ordered_count = schedule_count;
			n->rest_iter_count = chunk;

			*plower = my_lower;
			*pupper = my_upper;
			*pstride = incr;
			
			return 1;
			break;
		}
	}
}

/*for Critical directive -- By Cheng 06/05*/
inline void
__ompc_critical(int gtid, volatile ompc_lock_t **lock)
{
  mrapi_key_t lock_key_critical;
  mrapi_status_t mrapi_status;
#ifdef MRAPI 
  mrapi_mutex_lock(mutex_critical,&lock_key_critical, MRAPI_TIMEOUT_INFINITE, &mrapi_status);  //Cheng 06/05
  
  #ifdef DEBUG
    if(mrapi_status != MRAPI_SUCCESS)
      printf("failed to lock key..\n");
  #endif 
#endif 

#ifdef PTHREADS
    pthread_mutex_lock(&pthread_critical);
#endif

}

inline void
__ompc_end_critical(int gtid, volatile ompc_lock_t **lock)
{
  mrapi_key_t lock_key_critical;
  mrapi_status_t mrapi_status;
  
  //mrapi_mutex_hndl_t mutex_lck = mrapi_mutex_get(OMP_MCA_MUT_KEY_LCK_OFFSET,&mrapi_status);
#ifdef MRAPI
  mrapi_mutex_unlock(mutex_critical,&lock_key_critical, &mrapi_status);  //Cheng 06/05
  #ifdef DEBUG
    if(mrapi_status != MRAPI_SUCCESS)
      printf("failed to unlock key..\n");
  #endif 
#endif

#ifdef PTHREADS
    pthread_mutex_unlock(&pthread_critical);
#endif

}

omp_int32
__ompc_master (omp_int32 global_tid) 
{
  if (global_tid == 0) {
    return 1;
  }
  return 0;	
}

void 
__ompc_end_master (omp_int32 global_tid) 
{ 
  if(global_tid ==0) {
  }/* Do nothing*/
}


omp_int32 
__ompc_single (omp_int32 global_tid) 
{
  
  mrapi_key_t lock_key_single;
  mrapi_status_t mrapi_status;
  int single_count=0;
  //omp_team_t *p_team;
  //omp_v_thread_t *p_vthread;
  //mca_status_t mrapi_status;
  omp_icv_t* icvs = (omp_icv_t*)addr_data_root;  
  omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
	
  mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
  omp_mca_node_t* n = &(list_nodes[mrapi_node_id]);
    
  unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n->offset_list_team);
   
  omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n->num_team - 1]);

  omp_team_t* p_team = &(last_team[n->num_team - 1]);
  
  int	is_first = 0;

  if (n->omp_exe_mode == OMP_EXE_MODE_SEQUENTIAL)
    return 1;
 /* if (n->omp_exe_mode == OMP_EXE_MODE_NORMAL) {
    p_team = &__omp_level_1_team_manager;
    p_vthread = &__omp_level_1_team[global_tid];
  } else {
    p_vthread = __ompc_get_v_thread_by_num(global_tid);
    p_team = p_vthread->team;
  }
*/
  if (p_team->active_team_size == 1) {
    /* Single member team*/
    return 1;
  }

  single_count++;
  
#ifdef MRAPI
  mrapi_mutex_lock(mutex_single,&lock_key_single, MRAPI_TIMEOUT_INFINITE, &mrapi_status);  //Cheng 06/05
#endif

#ifdef PTHREADS
  pthread_mutex_lock(&pthread_single);
#endif
  //__ompc_lock(&(p_team->single_lock));
  if (global_single_count < single_count) {
    global_single_count++;
    is_first = 1;
  }
  //__ompc_unlock(&(p_team->single_lock));
#ifdef MRAPI
  mrapi_mutex_unlock(mutex_single,&lock_key_single, &mrapi_status);  //Cheng 06/05
#endif
#ifdef PTHREADS
  pthread_mutex_unlock(&pthread_single);
#endif
  return is_first;
}


void
__ompc_end_single (omp_int32 global_tid) 
{
  /* The single flags should be reset here*/
}


inline void
__ompc_reduction(int gtid, volatile ompc_lock_t **lck)
{
  mrapi_key_t lock_key_reduction;
  mrapi_status_t mrapi_status;
#ifdef MRAPI
  mrapi_mutex_lock(mutex_reduction,&lock_key_reduction, MRAPI_TIMEOUT_INFINITE, &mrapi_status);  //Cheng 06/05
  #ifdef DEBUG
    if(mrapi_status != MRAPI_SUCCESS)
      printf("failed to lock key..\n");
  #endif 
#endif
#ifdef PTHREADS
    pthread_mutex_lock(&pthread_reduction);
#endif

}

inline void
__ompc_end_reduction(int gtid, volatile ompc_lock_t **lck)
{
  mrapi_key_t lock_key_reduction;
  mrapi_status_t mrapi_status;
#ifdef MRAPI 
  mrapi_mutex_unlock(mutex_reduction,&lock_key_reduction, &mrapi_status);  //Cheng 06/05
  #ifdef DEBUG
    if(mrapi_status != MRAPI_SUCCESS)
      printf("failed to unlock key..\n");
  #endif 
#endif
#ifdef PTHREADS
    pthread_mutex_unlock(&pthread_reduction);
#endif


}

