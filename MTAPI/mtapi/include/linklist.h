#include <stdio.h>
#include <stdlib.h>
#include "mutex.h"
#define _GNU_SOURCE

/**
 * The unit object is a node in the linklist with a pointer to a void address and two pointers to its previous and next.
 */
typedef struct unit {
	void * ptr;
	struct unit * next;
	struct unit * prev;
} unit_t;
/**
 * The list object contains the head and tail of a linklist of unit object.
 */
typedef struct list {
	unsigned num_elements;
	unit_t *head;
	unit_t *tail;
	mutex_t mutex;
} list_t;

void list_init (list_t *);
void list_insert (list_t *, void *);
void list_delete(list_t *, unit_t *);

