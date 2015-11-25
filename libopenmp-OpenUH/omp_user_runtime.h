/* File : omp_user_runtime.h
 * Contents : user level function prototypes of the OpenMP routine
 * Contributor : Besar Wicaksono
 */

#ifndef OMP_USER_RUNTIME_H
#define OMP_USER_RUNTIME_H

#include <stdlib.h>

void omp_set_num_threads(int num);
int omp_get_num_threads(void);
int omp_get_max_threads(void);
int omp_get_thread_num(void);
int omp_get_num_procs(void);
int omp_in_parallel(void);
void omp_set_dynamic(int dynamic);
int omp_get_dynamic(void);
void omp_set_nested(int nested);
int omp_get_nested(void);
void* omp_allocate_shmem(size_t size);
void omp_deallocate_shmem(void* p);

#endif
