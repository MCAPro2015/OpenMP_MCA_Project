#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "threadpool.h"

#define TLS __thread

static int thread_id = 0;
static task_t * __first_task;
static pthread_mutex_t tid_mutex = PTHREAD_MUTEX_INITIALIZER;

TLS task_t *__current_task;
pool_t *__pool;
TLS int __master = 0;


static void threadpool_queue_init(queue_t * queue) 
{
	int i;
	for (i = 0; i < MAX_QUEUE_SIZE; i++) {
		queue->elements[i] = NULL;
	}
	queue->head = 0;
	queue->tail = 0;
	queue->num_elements = 0;
}

static int threadpool_queue_enqueue(queue_t *queue, void * data) 
{
	if (queue->num_elements == MAX_QUEUE_SIZE) {
		printf("queue is falllll unable to add elements\n");
		return 1;
	}

	queue->elements[queue->tail] = data;
	queue->num_elements++;
	queue->tail++;
	if (queue->tail == MAX_QUEUE_SIZE) 
		queue->tail = 0;
	return 0;
}

static void * threadpool_queue_dequeue(queue_t *queue) 
{
	void * data;
	if (queue->num_elements == 0) {
		printf ("empty queue \n");
		return NULL;
	}
	data =  queue->elements[queue->head];
	queue->elements[queue->head] = NULL;
	
	queue->num_elements--;
	if (queue->num_elements == 0) {
		queue->head = 0;
		queue->tail = 0;
	} else {
		queue->head++;
		if (queue->head == MAX_QUEUE_SIZE)
			queue->head = 0;
	}
	return data;
}

static int threadpool_queue_is_empty(queue_t *queue) {
	return (queue->num_elements == 0) ? 1:0;
}

static int threadpool_queue_getsize(queue_t *queue) 
{
	return queue->num_elements;
}

void threadpool_task_init(task_t *task) 
{
	task->routine = NULL;
	task->argument = NULL;
}

static task_t * threadpool_task_get_task(pool_t * pool)
{
	task_t * task;
	if (pool->stop_flag) {
		return NULL;
	}

	pthread_mutex_lock(&pool->mutex);
	/*
	while (threadpool_queue_is_empty(&(pool->task_queue))) {
		if (__master) {
			pthread_mutex_unlock(&pool->mutex);
			return NULL;
		}
		
		pthread_cond_wait(&pool->cond, &pool->mutex);
	}

	*/
	if (threadpool_queue_is_empty(&(pool->task_queue))) {
		pthread_mutex_unlock(&pool->mutex);
		return NULL;
	}
	if (pool->stop_flag) {
		pthread_mutex_unlock(&pool->mutex);
		return NULL;
	}

	task = threadpool_queue_dequeue(&pool->task_queue);
	pthread_mutex_unlock(&pool->mutex);
	return task;

}

static void * worker_routine (void *data)
{
	pool_t *pool = (pool_t *) data;
	task_t *task;
	__current_task = (task_t *) malloc (sizeof(task_t));
	__current_task->num_children = 0;
	__current_task->parent = NULL;

	while (1) {
		task = threadpool_task_get_task(pool);
		if (task == NULL) {
			if (pool->stop_flag) {
				/* work pool shut down */
				break;
			} else {
				continue;
			}
		}
	
		
		if (task->routine) {
			task->routine(task->argument);
			__sync_sub_and_fetch(&task->parent->num_children, 1);
			threadpool_task_init(task);

			pthread_mutex_lock(&pool->free_tasks_mutex);
			threadpool_queue_enqueue(&pool->free_task_queue, task);
			if (threadpool_queue_getsize(&pool->free_task_queue) == 1) {
				pthread_cond_broadcast(&pool->free_tasks_cond);
			}
			pthread_mutex_unlock(&pool->free_tasks_mutex);
		}
	}

	return NULL;
}


static void * stop_worker_routine(void *ptr) 
{
	int i;
	pool_t * pool =  (pool_t *) ptr;
	pool->stop_flag = 1;
	pthread_mutex_lock(&pool->mutex);
	pthread_cond_broadcast(&pool->cond);
	pthread_mutex_unlock(&pool->mutex);
	for (i = 0; i < pool->num_of_threads; i++){
		pthread_join(pool->tid[i], NULL);
	}
	free(pool->tid);
	free(pool);
	return NULL;
}

pool_t * threadpool_init(int num_of_threads) 
{
	pool_t * pool;
	int i;
	pool = (pool_t *) malloc (sizeof(pool_t));
	pool->stop_flag = 0;
	pthread_mutex_init(&pool->free_tasks_mutex, NULL);
       	pthread_mutex_init(&pool->mutex, NULL);
	pthread_mutex_init(&pool->tasks_mutex, NULL);
	pthread_cond_init(&pool->free_tasks_cond, NULL);
	pthread_cond_init(&pool->cond, NULL);

	threadpool_queue_init(&pool->task_queue);
	threadpool_queue_init(&pool->free_task_queue);
	
	for (i = 0; i < MAX_QUEUE_SIZE; i++) {
		threadpool_task_init((pool->tasks) + i);
		threadpool_queue_enqueue(&pool->free_task_queue, (pool->tasks) + i);
	}

	pool->tid = (pthread_t *) malloc (num_of_threads * sizeof (pthread_t));

	for (i = 0; i < num_of_threads; i++) {
		pthread_create(&pool->tid[i], NULL, worker_routine, pool);
	}
	pool->num_of_threads = num_of_threads;
	__pool = pool;

	__current_task = (task_t *) malloc (sizeof (task_t));
	__current_task->num_children = 0;
	__first_task = __current_task;
	__master = 1;
	return pool;
}

int threadpool_add_task (pool_t * pool, void (*routine)(void *), void * arg, int blocking)
{
	task_t * task;
	pthread_mutex_lock(&pool->free_tasks_mutex);

	while (threadpool_queue_is_empty(&pool->free_task_queue)) {
		if (!blocking) {
			pthread_mutex_unlock(&pool->free_tasks_mutex);
			return -2;
		} 
		pthread_cond_wait(&pool->free_tasks_cond, &pool->free_tasks_mutex);
	}

	task = (task_t *) threadpool_queue_dequeue(&pool->free_task_queue);
	pthread_mutex_unlock(&pool->free_tasks_mutex);
	task->argument = arg;
        task->routine = routine;
	task->parent =  __current_task;

	/* every task has a unique task id */
	pthread_mutex_lock(&tid_mutex);
	task->tid = thread_id++;
	pthread_mutex_unlock(&tid_mutex);


	pthread_mutex_lock(&pool->mutex);
	threadpool_queue_enqueue(&pool->task_queue, task);
	/* update number of children for current task */
	if (__current_task != NULL) {
		__sync_add_and_fetch(&__current_task->num_children, 1);
	}
	if (threadpool_queue_getsize(&pool->task_queue) == 1) {
		pthread_cond_broadcast(&pool->cond);
	}
	pthread_mutex_unlock(&pool->mutex);

	return 0;
}

void threadpool_free(pool_t * pool, int blocking)
{
	pthread_t tid;
	if (blocking) {
		stop_worker_routine(pool);
	} else {
		pthread_create(&tid, NULL, &stop_worker_routine, pool);
	}
}

void threadpool_task_wait()
{ 
//	sleep(1);
//	if (__master) 
//		sleep(99999);
	task_t * current_task, *next_task;
	current_task = __current_task;
	while (current_task->num_children) {
		pthread_mutex_lock(&__pool->tasks_mutex);
		printf ("");
		next_task = threadpool_task_get_task(__pool);
		pthread_mutex_unlock(&__pool->tasks_mutex);
		if (next_task != NULL) {
			threadpool_task_switch(next_task);
		}
		/*
		printf("current_task->children = %d\n", current_task->num_children);
		*/
	} 
}

void threadpool_task_switch(task_t * next_task) 
{
	task_t * orig_task, *current_task;
	orig_task = __current_task;
	__current_task = next_task;
        next_task->routine(next_task->argument);
	if (next_task->parent) {
		next_task->parent->num_children--;
	} else {
		printf("next_task->parent == NULL\n");
	}
	__current_task = orig_task;	
	current_task = __current_task;
}































































































