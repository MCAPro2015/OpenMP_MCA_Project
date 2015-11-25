#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include "mutex.h"
#define _GNU_SOURCE
#define MAX_QUEUE_SIZE  1000
/**
 * The queue object maintains a FIFO queue of pointers.
 */

typedef struct queue  {
	unsigned int head;
	unsigned int tail;
	unsigned int num_elements;
	void * elements[MAX_QUEUE_SIZE];
	mutex_t mutex;
} queue_t;

void queue_init(queue_t *);
int queue_enqueue(queue_t *, void *);
void * queue_dequeue(queue_t *);
int queue_is_empty(queue_t *);
int queue_getsize(queue_t *);
int queue_maxsize();
void queue_lock();
void queue_unlock();
int queue_is_full(queue_t *);




































