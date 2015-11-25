#include "linklist.h"
/**
 * Initialize an link list.
 */
void list_init(list_t * list) 
{
	list->head = NULL;
	list->tail = NULL;
	list->num_elements = 0;
	mutex_init(&list->mutex);
	return;
}

/**
 * Insert a node to the tail of an existing link list.
 */

void list_insert(list_t * list, void * ptr) 
{
	unit_t * newUnit = (unit_t *) malloc (sizeof (unit_t));
	newUnit->ptr = ptr;
	mutex_lock(&list->mutex);
	if (list->head == NULL) {
		newUnit->next = NULL;
		newUnit->prev = NULL;	
		list->head = newUnit;
		list->tail = newUnit;
		list->num_elements++;
	} else {
		newUnit->prev = list->tail;
		list->tail->next = newUnit;
		list->tail = newUnit;
		newUnit->next = NULL;
		list->num_elements++;
	}
	mutex_unlock(&list->mutex);
	return ;
}

/**
 * Delete a node from the link list.
 */

void list_delete(list_t * list, unit_t *unitPtr)
{
	mutex_lock(&list->mutex);
	if (list->head == unitPtr) {
		list->head = unitPtr->next;
		if (list->head)
			list->head->prev = NULL;
	} else if (list->tail == unitPtr) {
		list->tail = list->tail->prev;
		list->tail->next = NULL;
	
	} else {
		unitPtr->prev->next = unitPtr->next;
		unitPtr->next->prev = unitPtr->prev;
	}
	list->num_elements--;
	free(unitPtr);
	mutex_unlock(&list->mutex);
}


