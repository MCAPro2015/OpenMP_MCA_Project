#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/**
 * Initialize an empty queue for pointers 
 */

void queue_init(queue_t * queue) 
{
	int i;
	for (i = 0; i < MAX_QUEUE_SIZE; i++) {
		queue->elements[i] = NULL;
	}
	queue->head = 0;
	queue->tail = 0;
	queue->num_elements = 0;
	mutex_init(&queue->mutex);
}

/**
 * Insert an element into a existing queue
 */

int queue_enqueue(queue_t *queue, void * data) 
{
	if (queue->num_elements == MAX_QUEUE_SIZE) {
		printf("queue is full unable to add elements\n");
		return 1;
	}

	queue->elements[queue->tail] = data;
	queue->num_elements++;
	queue->tail++;
	if (queue->tail == MAX_QUEUE_SIZE) 
		queue->tail = 0;
	return 0;
}

/**
 * Dequeue an element from an existing queue
 */

void * queue_dequeue(queue_t *queue) 
{
	void * data;
	if (queue->num_elements == 0) {
	//	printf ("empty queue \n");
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

/**
 * If the queue is empty return 1 otherwise return 0.
 */

int queue_is_empty(queue_t *queue) {
	return (queue->num_elements == 0) ? 1:0;
}

/**
 * If the queue is full return 1 otherwise return 0.
 */

int queue_is_full(queue_t * queue) 
{
	return (queue->num_elements == MAX_QUEUE_SIZE) ? 1:0;
}

/**
 * Return the size of an existing queue.
 */

int queue_getsize(queue_t *queue) 
{
	return queue->num_elements;
}

/**
 * Return the maximum volumn of an queue object.
 */

int queue_maxsize()
{
	return MAX_QUEUE_SIZE;
}
/**
 * Lock the queue.
 */
void queue_lock(queue_t *queue)
{
	mutex_lock(&queue->mutex);
}

/**
 * Unlock the queue
 */
void queue_unlock(queue_t *queue)
{
	mutex_unlock(&queue->mutex);
}



