// both node 0 and node 1 works on fib
#include "fib.h"
#include "time.h"
#include "sys/time.h"
#define NUM_THREADS 4

#define DOMAIN 0
#define NODE 0

static int tid = 0;


void actionFunction0 (
	MTAPI_IN void *arg,
	MTAPI_IN mtapi_size_t arg_size,
	MTAPI_OUT void *result_buffer,
	MTAPI_IN mtapi_size_t result_buffer_size,
	MTAPI_IN void * node_local_data,
	MTAPI_IN mtapi_size_t node_local_data_size,
	mtapi_task_context_t * const context
		)
{
	int n;
	int * result;
	result = (int *) result_buffer;
	n = *(int *) arg;
	*result = 2 * n;
}

void actionFunction1 (
	MTAPI_IN void *arg,
	MTAPI_IN mtapi_size_t arg_size,
	MTAPI_OUT void *result_buffer,
	MTAPI_IN mtapi_size_t result_buffer_size,
	MTAPI_IN void * node_local_data,
	MTAPI_IN mtapi_size_t node_local_data_size,
	mtapi_task_context_t * const context
		)
{
	mtapi_job_hndl_t jobHndl;
	mtapi_task_hndl_t taskHndl;
	mtapi_group_hndl_t groupHndl = {MTAPI_GROUP_NONE};
	mtapi_status_t mtapi_status;

	int n;
	int * result;
	result = (int *) result_buffer;
	n = *(int *) arg;
	printf("processing fib(%d)\n", n);
	if (n < 2) {
		* result = n;
	} else {
		int a, b;
		int x, y;
		a = n - 1;
		b = n - 2;
		jobHndl = mtapi_job_get(
				FUNCTION_1,
				&mtapi_status
				);
		taskHndl = mtapi_task_start (
				tid++,
				jobHndl,
				(void *) &a,
				sizeof(int),
				(void *) &x,
				sizeof(int),
				MTAPI_DEFAULT_TASK_ATTRIBUTES,
				groupHndl,
				&mtapi_status
				);
//		actionFunction2((void *) &a, sizeof(int), (void *)&x, sizeof(int), NULL, 0, NULL);
		mtapi_task_wait(taskHndl, MTAPI_TIMEOUT_INFINITE, &mtapi_status);
		actionFunction1((void *) &b, sizeof(int), (void *)&y, sizeof(int), NULL, 0, NULL);
		* result = x + y;
	}

}

int main()
{
	
//	int tid = 0;
	int n, n2;
	int result, result2;
	int node, node2;
	mcapi_status_t mcapi_status;
	mtapi_info_t mtapi_info;
	mtapi_status_t mtapi_status;
	mtapi_job_hndl_t jobHndl;
	mtapi_task_hndl_t taskHndl;
	mtapi_group_hndl_t groupHndl;
	
	struct timeval start, end;	
	printf("Select processor #(0 or 1)\n");
	scanf("%d", &node);
	if (node == 0) {
		printf("Input fib number: \n");
		scanf("%d", &n);

		mtapi_initialize (0, 0, NULL, &mtapi_info, &mtapi_status);
		mtapi_action_create(
			FUNCTION_1,
			(actionFunction1), 
			NULL,
			0,
			NULL,
			&mtapi_status
			);
		jobHndl = mtapi_job_get(
			FUNCTION_1,
			&mtapi_status);
		gettimeofday(&start, NULL);
		taskHndl = mtapi_task_start (
			tid++,
			jobHndl,
			(void *) &n,
			sizeof(int),
			(void *) &result,
			sizeof(int),
			MTAPI_DEFAULT_TASK_ATTRIBUTES,
			groupHndl,
			&mtapi_status
			);
		mtapi_task_wait(taskHndl, MTAPI_TIMEOUT_INFINITE, &mtapi_status);
		gettimeofday(&end, NULL);
		printf("fib(%d) = %d\n", n, result);
		printf("time = %f seconds\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000.0);
		mcapi_finalize(&mcapi_status);
	} else 
	{
		mtapi_initialize (1, 0, NULL, &mtapi_info, &mtapi_status);
		mtapi_action_create(
			FUNCTION_1,
			(actionFunction1),
			NULL,
			0,
			NULL,
			&mtapi_status
			);
		while(1) {
			sched_yield();
		}
	}
	return 0;
}
