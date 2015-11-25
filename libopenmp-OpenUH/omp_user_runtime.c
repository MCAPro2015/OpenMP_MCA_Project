#include "omp_user_runtime.h"
#include "omp_rtl.h"

inline void omp_set_num_threads(int num)
{
    int num_threads;
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    
    // check the validity of _num_threads
    num_threads = __ompc_check_num_threads(num);
    icvs->__omp_nthreads_var = num_threads; 
}

int omp_get_num_threads(void)
{
    mca_status_t mrapi_status;
    
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t n = list_nodes[mrapi_node_id];
    
    unsigned long* offset_list_team = (unsigned long*)((unsigned long)addr_data_root + n.offset_list_team);
	omp_team_t* last_team = (omp_team_t*)((unsigned long)addr_data_root + offset_list_team[n.num_team - 1]);
    if(n.omp_exe_mode == OMP_EXE_MODE_SEQUENTIAL){
        return 1;
    }
    else{
        return last_team->active_team_size;
    }
}

int omp_get_max_threads(void)
{
    /* returns the maximum number of threads that can execute concurrently in a single parallel region */
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    return icvs->__omp_max_num_threads;
}

int omp_get_thread_num(void)
{
    mca_status_t mrapi_status;
    
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t n = list_nodes[mrapi_node_id];
    
//    int vthread_id = n.node_id - n.last_team->list_vthread[0].node_id;
	int* list_vthread_id = (int*)((unsigned long)addr_data_root + n.offset_list_vthread_id);
    int vthread_id = list_vthread_id[n.num_team - 1];
//    printf("nodeid-%d vthreadid-%d\n",n.node_id,vthread_id);
    return vthread_id;
}

int omp_get_num_procs(void)
{
	omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    return icvs->__omp_num_cpus;
}

int omp_in_parallel(void)
{
    mca_status_t mrapi_status;
    
    omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    omp_mca_node_t* list_nodes = (omp_mca_node_t*)((unsigned long)addr_data_root + icvs->offset_list_nodes);
    
    mrapi_node_t mrapi_node_id = mrapi_node_id_get(&mrapi_status);
    omp_mca_node_t n = list_nodes[mrapi_node_id];
    
    return (n.omp_exe_mode != OMP_EXE_MODE_SEQUENTIAL);
}

void omp_set_dynamic(int dynamic)
{
	omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    icvs->__omp_dynamic = dynamic;
}

int omp_get_dynamic(void)
{
	omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    return icvs->__omp_dynamic;
}

void omp_set_nested(int nested)
{
	omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    icvs->__omp_nested = nested;
}

int omp_get_nested(void)
{
	omp_icv_t* icvs = (omp_icv_t*)addr_data_root;
    return icvs->__omp_nested;
}

void* omp_allocate_shmem_offset(size_t size)
{
	unsigned long offset = __ompc_allocate(size);
	unsigned long res = ((unsigned long)addr_data_root + offset);
	// printf("addr_data_root : %p offset : %p res : %p\n",addr_data_root,offset,res);
	return (void*)(offset);
}

void omp_deallocate_shmem_offset(void* p)
{
	__ompc_deallocate_offset((unsigned long)p);
}

void* omp_get_effective_addr(void* addr)
{
	return (void*)((unsigned long)addr_data_root + (unsigned long)addr);
}


