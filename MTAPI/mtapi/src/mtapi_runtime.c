#include "mcapi.h"
#include "mca_config.h"
#include "mtapi_runtime.h"
#include "mcapi_impl_spec.h"


TLS mtapiRT_TaskInfo_t * currentTask = NULL;
TLS m_bit = 0x0000; /**< The core affinity bit */
mtapi_boolean_t nodeInit = MTAPI_FALSE; 
mtapi_node_attributes_t * nodeAttr;
int sendCnt;
int recvCnt;
int waitCnt;
int awaitCnt;
int completeLocalCnt;
void mtapiRT_initialize (mtapi_domain_t domainID, mtapi_node_t nodeID) {
	int i;
	int rc;
	_domainID = domainID;
	_nodeID = nodeID;
	

	pthread_t tid[NUM_THREAD];
	pthread_t listenerID;
	pthread_t recvID;
	int idx[NUM_THREAD];
	// initialize the jog-action look-up table 
	for (i = 0; i < NUM_JOB; i++) {
		jobList[i].actionList = NULL;	
	}	

	// initialize node information as input args for threads.
	mtapiRT_NodeInfo_t * nodeInfo = (mtapiRT_NodeInfo_t *) malloc (sizeof(mtapiRT_NodeInfo_t)); 
	nodeInfo->domainID = domainID;
	nodeInfo->nodeID = nodeID;	
	// initialize the task list and the mutex lock 
	taskListHead = NULL;
	taskListTail = NULL;
	pthread_mutex_init(&taskListMutex, NULL);

	// initialize the message list and its mutex;
	messageListHead = NULL;
	messageListTail = NULL;
	pthread_mutex_init (&messageListMutex, NULL);
	waitCnt = 0;
	completeLocalCnt = 0;
	awaitCnt = 0;
	// initialize node information;
	for (i = 0; i < NODE_COUNT; i++) {
		nodeList[i].domainID = NODE_INFOS[2*i];
	        nodeList[i].nodeID = NODE_INFOS[2*i+1];	
	}
	currentTask = NULL;
	sendCnt = 0;
	recvCnt = 0;
	for (i = 0; i < NUM_THREAD; i++) {
		idx[i] = i;
		rc = pthread_create(&tid[i], NULL, thread_func, (void *)&idx[i]);
		if (rc) {
			printf("create thread failure\n");
		}
	}
	// initialize listener_func
	if (NODE_COUNT > 1) {
		rc = pthread_create(&listenerID, NULL, listener_func, NULL);	
		rc = pthread_create(&recvID, NULL, processer_func, NULL);
	}
	nodeInit = MTAPI_TRUE;
	// currentTask = NULL;
}


mtapiRT_JobInfo_t * mtapiRT_getJob (mtapi_job_id_t job_id) {
	return &jobList[job_id];
}

mtapiRT_ActionInfo_t * mtapiRT_createAction (
		mtapi_job_id_t job_id,
		mtapi_action_function_t function
		) {
	mtapiRT_ActionInfo_t * action = (mtapiRT_ActionInfo_t *) malloc (sizeof(mtapiRT_ActionInfo_t));
	action->jobID = job_id;
	action->type = MTAPIRT_ACTION_LOCAL;
	action->location.fptr = function;
	action->next = NULL;
	if (jobList[job_id].actionList == NULL) {
		jobList[job_id].actionList = action;
	} else {
		mtapiRT_ActionInfo_t * ptr1 = jobList[job_id].actionList;
		mtapiRT_ActionInfo_t * ptr2 = ptr1->next;
		while (ptr2) {
			ptr1 = ptr1->next;
			ptr2 = ptr1->next;
		}
		ptr1->next = action;
	}
	// broadcast action to other node.
	if (NODE_COUNT > 1) {
		mtapiRT_Message_t * message = (mtapiRT_Message_t * ) malloc (sizeof(mtapiRT_Message_t));
		message->senderDomain = _domainID;
		message->senderNode = _nodeID;
		message->senderPort = PORTID;
		mtapi_job_id_t * jobID = (mtapi_job_id_t *) malloc (sizeof(mtapi_job_id_t));
		*jobID = job_id;
		message->buffer = (void *) jobID;
		message->bufferSize = sizeof(mtapi_job_id_t);
		pthread_mutex_lock(&messageListMutex);
		message->type = MTAPIRT_MSG_CREATE_ACTION;
		InsertMessage(message);
		pthread_mutex_unlock(&messageListMutex);	
	}
	return action;
}

mtapiRT_ActionInfo_t * mtapiRT_createActionDummy (
	mtapi_domain_t domainID,
	mtapi_node_t nodeID,
	mtapi_job_id_t job_id
) {
	mtapiRT_ActionInfo_t * action = (mtapiRT_ActionInfo_t *) malloc (sizeof(mtapiRT_ActionInfo_t));
	action->jobID = job_id;
	action->type = MTAPIRT_ACTION_REMOTE;
	// location info
	action->location.remoteNode.domain = 1;
	action->location.remoteNode.node = 1;
	action->location.remoteNode.port = PORTID;
	action->next = NULL;
	if (jobList[job_id].actionList == NULL) {
		jobList[job_id].actionList = action;
	} else {
		mtapiRT_ActionInfo_t * ptr1 = jobList[job_id].actionList;
		mtapiRT_ActionInfo_t * ptr2 = ptr1->next;
		while (ptr2) {
			ptr1 = ptr1->next;
			ptr2 = ptr1->next;
		}
		ptr1->next = action;
	}
	return action;
}
// TODO local and remote
mtapiRT_ActionInfo_t * mtapiRT_getAction (
		mtapiRT_JobInfo_t * job
) {
	while (1) {
		if (job->actionList) {
			return job->actionList;
		}
	}
}
mtapiRT_TaskInfo_t * mtapiRT_startTask (
		mtapi_task_id_t task_id,
		mtapiRT_JobInfo_t * jobPtr,
		MTAPI_IN void * arguments,
		MTAPI_IN size_t arguments_size,
		MTAPI_OUT void * result,
		MTAPI_IN size_t result_size,
		mtapiRT_GroupInfo_t * group,
//		mtapi_boolean_t isFromRemote,
//		mtapiRT_LocationInfo_t * location,
		mtapiRT_Message_t * message
		) {
	int rc;
	mtapiRT_TaskInfo_t * task = (mtapiRT_TaskInfo_t *) malloc (sizeof(mtapiRT_TaskInfo_t));
	task->action = mtapiRT_getAction(jobPtr);
	task->context = (mtapi_task_context_t *) malloc (sizeof(mtapi_task_context_t));
	task->context->status = MTAPI_TASK_CREATED;
	task->arguments = arguments;
	task->arguments_size = arguments_size;
	task->result = result;
	task->result_size = result_size;
	task->next = NULL;
	task->parent = currentTask;
	task->message = message;	// associate with message
	if (task->parent) {
		__sync_fetch_and_add(&task->parent->childCount, 1);
	}
	task->childCount = 0;
	// ordinary task without a task queue option.
	task->queue = NULL;

	// increase the group count. 
	task->group = group;
	if (task->group) {	// task belongs to a group
		__sync_fetch_and_add(&task->group->currentNumOfTask, 1);
		__sync_fetch_and_add(&task->group->numOfTask, 1);
	}
	// if task is from remote. Needs to be sent back.
//	if (isFromRemote) {
//		task->location = location;
//	} else {
//		task->location = NULL;
//	}
	pthread_mutex_lock(&taskListMutex);
	rc = InsertTask(task);
	pthread_mutex_unlock(&taskListMutex);
//	printf("Insert task %d\n", *(int *)task->arguments);
	if (rc) {
		printf("task is NULL");
	}
	return task;
}

void mtapiRT_waitForTask(mtapiRT_TaskInfo_t * taskPtr) { 
	mtapiRT_TaskInfo_t * nextTask;
	int cnt = 0;
	while (1) {
		if (taskPtr->context->status == MTAPI_TASK_COMPLETED) {
			free(taskPtr->context);
			free(taskPtr);
			break;
		} else {
		
			pthread_mutex_lock(&taskListMutex);
			nextTask = GetTaskFromHead();
			pthread_mutex_unlock(&taskListMutex);
			ProcessTask(nextTask);
		}
	}
}

mtapiRT_QueueInfo_t * mtapiRT_createQueue (
		mtapi_queue_id_t queue_id,
		mtapiRT_JobInfo_t * job
		) {
	mtapiRT_QueueInfo_t * queue = (mtapiRT_QueueInfo_t * ) malloc (sizeof(mtapiRT_QueueInfo_t));
	queue->id = queue_id;
	queue->job = job;
	queue->head = NULL;
	queue->tail = NULL;
	pthread_mutex_init(&queue->mutex, NULL);
	return queue;
}

mtapiRT_TaskInfo_t * mtapiRT_enqueueTask (
		mtapi_task_id_t task_id,
		mtapiRT_QueueInfo_t * queue,
		MTAPI_IN void * arguments,
		MTAPI_IN size_t arguments_size,
		MTAPI_OUT void * result,
		MTAPI_IN size_t result_size,
		mtapiRT_GroupInfo_t * group
		) {
	int rc;
	mtapiRT_TaskInfo_t * task = (mtapiRT_TaskInfo_t *) malloc (sizeof(mtapiRT_TaskInfo_t));
	task->action = mtapiRT_getAction(queue->job);
	task->context = (mtapi_task_context_t *) malloc (sizeof(mtapi_task_context_t));
	task->context->status = MTAPI_TASK_CREATED;
	task->arguments = arguments;
	task->arguments_size = arguments_size;
	task->result = result;
	task->result_size = result_size;
	task->next = NULL;
	task->parent = currentTask;
	if (task->parent) {
		__sync_fetch_and_add(&task->parent->childCount, 1);
	}
	task->childCount = 0;
	// specific for the queue.
	task->queue = queue;
	// create an task entry first. 
	mtapiRT_TaskEntry_t * taskEntry = (mtapiRT_TaskEntry_t *) malloc (sizeof(mtapiRT_TaskEntry_t));
	taskEntry->task = task;
	taskEntry->next = NULL;
	pthread_mutex_lock(&task->queue->mutex);
	if (task->queue->head == NULL) { // first task in queue
		task->queue->head = taskEntry;
		task->queue->tail = taskEntry;
	} else {	// insert at tail.
		task->queue->tail->next = taskEntry;
		task->queue->tail = taskEntry;
		
	}
	// insert into task list must always be the last step
	pthread_mutex_unlock(&task->queue->mutex);
	// task group 
	task->group = group;
	// TODO finish the enqueue after finish starttask
	task->message = NULL;
	if (task->group) {	// task belongs to a group
		__sync_fetch_and_add(&task->group->currentNumOfTask, 1);
		__sync_fetch_and_add(&task->group->numOfTask, 1);
	}

	pthread_mutex_lock(&taskListMutex);
	rc = InsertTask(task);
	pthread_mutex_unlock(&taskListMutex);
	
	if (rc) {
		printf("task is NULL");
	}
	return task;
}

mtapiRT_GroupInfo_t * mtapiRT_createGroup (mtapi_group_id_t id) {
	mtapiRT_GroupInfo_t * group = (mtapiRT_GroupInfo_t *) malloc (sizeof(mtapiRT_GroupInfo_t));
	group->id = id;
	group->numOfTask = 0;
	group->currentNumOfTask = 0;
	group->result = NULL;
	
}

void mtapiRT_waitForGroupAll (
		mtapiRT_GroupInfo_t * group
) { 
	mtapiRT_TaskInfo_t * nextTask;
	while (1) {
		// TODO __sync_bool_compare_and_swap
		if (__sync_bool_compare_and_swap(&group->currentNumOfTask, 0, 0)) {
			break;
		} else {
			pthread_mutex_lock(&taskListMutex);
			nextTask = GetTaskFromHead();
			pthread_mutex_unlock(&taskListMutex);
			ProcessTask(nextTask);
		}	
	}
}

void mtapiRT_waitForGroupAny (
		mtapiRT_GroupInfo_t * group,
		void ** result
) {
	mtapiRT_TaskInfo_t * nextTask;
	while (1) {
		// TODO __sync_bool_compare_and_swap
		if (group->currentNumOfTask < group->numOfTask) {
			if (result) {
				*result = group->result;
			}
			break;
		} else {
			pthread_mutex_lock(&taskListMutex);
			nextTask = GetTaskFromHead();
			pthread_mutex_unlock(&taskListMutex);
			ProcessTask(nextTask);
		}	
	}
}


// Copy
mtapi_boolean_t mtapiRT_nodeInitialized (mtapi_status_t * status)
{
	*status = MTAPI_SUCCESS;
	if (nodeInit) {
		return MTAPI_TRUE;
	}
	return MTAPI_FALSE;
}

void mtapiRT_initNodeAttribute (
	MTAPI_OUT mtapi_node_attributes_t * attributes,
	MTAPI_OUT mtapi_status_t * status
	)
{
	memset(attributes, 0, sizeof(mtapi_node_attributes_t));
	*status = MTAPI_SUCCESS;
}

void mtapiRT_setNodeAttribute (
	MTAPI_INOUT mtapi_node_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_IN void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	if (attribute_size != sizeof(mtapi_uint_t)) {
		* status = MTAPI_ERR_ATTR_SIZE;
		return;
	}
	switch (attribute_num) {
		case MTAPI_NODE_NUMCORES :
			attributes->entries[attribute_num].valid = MTAPI_TRUE;
			attributes->entries[attribute_num].attribute_num = attribute_num;
			attributes->entries[attribute_num].bytes = attribute_size;
			attributes->entries[attribute_num].attribute_d = malloc (attribute_size);
			memcpy(attributes->entries[attribute_num].attribute_d, attribute, attribute_size);
			* status = MTAPI_SUCCESS;
			break;
		default:
			*status =  MTAPI_ERR_ATTR_NUM;
			break;
	}
}

void mtapiRT_getNodeAttribute (
	MTAPI_IN mtapi_node_t node,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_OUT void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	switch (attribute_num) {
		case MTAPI_NODE_NUMCORES :
			if (attribute_size != sizeof(mtapi_uint_t)) {
				* status = MTAPI_ERR_ATTR_SIZE;
				return;
			}
			if (nodeAttr) {
				memcpy (attribute, nodeAttr->entries[attribute_num].attribute_d, attribute_size);
				* status = MTAPI_SUCCESS;
				break;
			} else {
				printf("node attribute NULL\n");
				* status = MTAPI_SUCCESS;
				break;
			}
		default:
			*status = MTAPI_ERR_ATTR_NUM;
			break;
	}
}

void mtapiRT_initActionAttribute (
	MTAPI_OUT mtapi_action_attributes_t * attributes,
	MTAPI_OUT mtapi_status_t * status
	)
{
	if (mtapiRT_nodeInitialized(status)) {
		memset(attributes, 0, sizeof(mtapi_action_attributes_t));
		*status = MTAPI_SUCCESS;
	} else {
		* status = MTAPI_ERR_NODE_NOTINIT;
	}
}
void mtapiRT_setActionAttribute (
	MTAPI_INOUT mtapi_action_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_IN void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	switch (attribute_num) {
		case MTAPI_ACTION_GLOBAL :
			if (attribute_size != sizeof(mtapi_boolean_t)) {
				printf("attribute size dont match\n");
				* status = MTAPI_ERR_ATTR_SIZE;
				return; 
			}
			attributes->entries[attribute_num].valid = MTAPI_TRUE;
			attributes->entries[attribute_num].attribute_num = attribute_num;
			attributes->entries[attribute_num].bytes = attribute_size;
			attributes->entries[attribute_num].attribute_d = malloc (attribute_size);
			memcpy(attributes->entries[attribute_num].attribute_d, attribute, attribute_size);
			* status = MTAPI_SUCCESS;
			break;

		case MTAPI_ACTION_AFFINITY :
			if (attribute_size != sizeof(mtapi_affinity_t)) {
				printf("attribute size dont match\n");
				* status = MTAPI_ERR_ATTR_SIZE;
				return; 
			}
			attributes->entries[attribute_num].valid = MTAPI_TRUE;
			attributes->entries[attribute_num].attribute_num = attribute_num;
			attributes->entries[attribute_num].bytes = attribute_size;
			attributes->entries[attribute_num].attribute_d = malloc (attribute_size);
			memcpy(attributes->entries[attribute_num].attribute_d, attribute, attribute_size);
			* status = MTAPI_SUCCESS;
			break;

		case MTAPI_DOMAIN_SHARED :
			if (attribute_size != sizeof(mtapi_boolean_t)) {
				printf("attribute size dont match\n");
				* status = MTAPI_ERR_ATTR_SIZE;
				return; 
			}
			attributes->entries[attribute_num].valid = MTAPI_TRUE;
			attributes->entries[attribute_num].attribute_num = attribute_num;
			attributes->entries[attribute_num].bytes = attribute_size;
			attributes->entries[attribute_num].attribute_d = malloc (attribute_size);
			memcpy(attributes->entries[attribute_num].attribute_d, attribute, attribute_size);
			* status = MTAPI_SUCCESS;
			break;
		default:
			*status =  MTAPI_ERR_ATTR_NUM;
			break;
	}
}
void mtapiRT_getActionAttribute (
	MTAPI_IN mtapi_action_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_OUT void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	switch (attribute_num) {
		case MTAPI_ACTION_GLOBAL :
			if (attribute_size != sizeof(mtapi_boolean_t)) {
				* status = MTAPI_ERR_ATTR_SIZE;
			} else  {
				memcpy (attribute, attributes->entries[attribute_num].attribute_d, attribute_size);
				* status = MTAPI_SUCCESS;
			}
			break;
		case MTAPI_ACTION_AFFINITY :
			if (attribute_size != sizeof(mtapi_affinity_t)) {
				* status = MTAPI_ERR_ATTR_SIZE;
			} else  {
				memcpy (attribute, attributes->entries[attribute_num].attribute_d, attribute_size);
				* status = MTAPI_SUCCESS;
			}
			break;
		case MTAPI_DOMAIN_SHARED :
			if (attribute_size != sizeof(mtapi_boolean_t)) {
				* status = MTAPI_ERR_ATTR_SIZE;
			} else  {
				memcpy (attribute, attributes->entries[attribute_num].attribute_d, attribute_size);
				* status = MTAPI_SUCCESS;
			}
			break;
		default:
			*status = MTAPI_ERR_ATTR_NUM;
			break;
	}
}
void mtapiRT_initQueueAttribute (
	MTAPI_OUT mtapi_queue_attributes_t * attributes,
	MTAPI_OUT mtapi_status_t * status
	)
{
	if (mtapiRT_nodeInitialized(status)) {
		memset(attributes, 0, sizeof(mtapi_queue_attributes_t));
		*status = MTAPI_SUCCESS;
	} else {
		* status = MTAPI_ERR_NODE_NOTINIT;
	}
}
void mtapiRT_setQueueAttribute (
	MTAPI_INOUT mtapi_queue_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_IN void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	switch (attribute_num) {
		case MTAPI_QUEUE_GLOBAL :
			if (attribute_size != sizeof(mtapi_boolean_t)) {
				printf("attribute size dont match\n");
				* status = MTAPI_ERR_ATTR_SIZE;
				return; 
			}
			attributes->entries[attribute_num].valid = MTAPI_TRUE;
			attributes->entries[attribute_num].attribute_num = attribute_num;
			attributes->entries[attribute_num].bytes = attribute_size;
			attributes->entries[attribute_num].attribute_d = malloc (attribute_size);
			memcpy(attributes->entries[attribute_num].attribute_d, attribute, attribute_size);
			* status = MTAPI_SUCCESS;
			break;

		case MTAPI_QUEUE_PRIORITY :
			if (attribute_size != sizeof(mtapi_uint_t)) {
				printf("attribute size dont match\n");
				* status = MTAPI_ERR_ATTR_SIZE;
				return; 
			}
			attributes->entries[attribute_num].valid = MTAPI_TRUE;
			attributes->entries[attribute_num].attribute_num = attribute_num;
			attributes->entries[attribute_num].bytes = attribute_size;
			attributes->entries[attribute_num].attribute_d = malloc (attribute_size);
			memcpy(attributes->entries[attribute_num].attribute_d, attribute, attribute_size);
			* status = MTAPI_SUCCESS;
			break;

		case MTAPI_QUEUE_LIMIT :
			if (attribute_size != sizeof(mtapi_uint_t)) {
				printf("attribute size dont match\n");
				* status = MTAPI_ERR_ATTR_SIZE;
				return; 
			}
			attributes->entries[attribute_num].valid = MTAPI_TRUE;
			attributes->entries[attribute_num].attribute_num = attribute_num;
			attributes->entries[attribute_num].bytes = attribute_size;
			attributes->entries[attribute_num].attribute_d = malloc (attribute_size);
			memcpy(attributes->entries[attribute_num].attribute_d, attribute, attribute_size);
			* status = MTAPI_SUCCESS;
			break;
		case MTAPI_QUEUE_ORDERED :
			if (attribute_size != sizeof(mtapi_boolean_t)) {
				printf("attribute size dont match\n");
				* status = MTAPI_ERR_ATTR_SIZE;
				return; 
			}
			attributes->entries[attribute_num].valid = MTAPI_TRUE;
			attributes->entries[attribute_num].attribute_num = attribute_num;
			attributes->entries[attribute_num].bytes = attribute_size;
			attributes->entries[attribute_num].attribute_d = malloc (attribute_size);
			memcpy(attributes->entries[attribute_num].attribute_d, attribute, attribute_size);
			* status = MTAPI_SUCCESS;
			break;

		case MTAPI_QUEUE_RETAIN :
			if (attribute_size != sizeof(mtapi_boolean_t)) {
				printf("attribute size dont match\n");
				* status = MTAPI_ERR_ATTR_SIZE;
				return; 
			}
			attributes->entries[attribute_num].valid = MTAPI_TRUE;
			attributes->entries[attribute_num].attribute_num = attribute_num;
			attributes->entries[attribute_num].bytes = attribute_size;
			attributes->entries[attribute_num].attribute_d = malloc (attribute_size);
			memcpy(attributes->entries[attribute_num].attribute_d, attribute, attribute_size);
			* status = MTAPI_SUCCESS;
			break;

		case MTAPI_DOMAIN_SHARED :
			if (attribute_size != sizeof(mtapi_boolean_t)) {
				printf("attribute size dont match\n");
				* status = MTAPI_ERR_ATTR_SIZE;
				return; 
			}
			attributes->entries[attribute_num].valid = MTAPI_TRUE;
			attributes->entries[attribute_num].attribute_num = attribute_num;
			attributes->entries[attribute_num].bytes = attribute_size;
			attributes->entries[attribute_num].attribute_d = malloc (attribute_size);
			memcpy(attributes->entries[attribute_num].attribute_d, attribute, attribute_size);
			* status = MTAPI_SUCCESS;
			break;
		default:
			*status =  MTAPI_ERR_ATTR_NUM;
			break;
	}
}

void mtapiRT_getQueueAttribute (
	MTAPI_IN mtapi_queue_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_OUT void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	switch (attribute_num) {
		case MTAPI_QUEUE_GLOBAL :
			if (attribute_size != sizeof(mtapi_boolean_t)) {
				* status = MTAPI_ERR_ATTR_SIZE;
			} else  {
				memcpy (attribute, attributes->entries[attribute_num].attribute_d, attribute_size);
				* status = MTAPI_SUCCESS;
			}
			break;
		case MTAPI_QUEUE_PRIORITY :
			if (attribute_size != sizeof(mtapi_uint_t)) {
				* status = MTAPI_ERR_ATTR_SIZE;
			} else  {
				memcpy (attribute, attributes->entries[attribute_num].attribute_d, attribute_size);
				* status = MTAPI_SUCCESS;
			}
			break;
		case MTAPI_QUEUE_LIMIT :
			if (attribute_size != sizeof(mtapi_uint_t)) {
				* status = MTAPI_ERR_ATTR_SIZE;
			} else  {
				memcpy (attribute, attributes->entries[attribute_num].attribute_d, attribute_size);
				* status = MTAPI_SUCCESS;
			}
			break;
		case MTAPI_QUEUE_ORDERED :
			if (attribute_size != sizeof(mtapi_boolean_t)) {
				* status = MTAPI_ERR_ATTR_SIZE;
			} else  {
				memcpy (attribute, attributes->entries[attribute_num].attribute_d, attribute_size);
				* status = MTAPI_SUCCESS;
			}
			break;
		case MTAPI_QUEUE_RETAIN :
			if (attribute_size != sizeof(mtapi_uint_t)) {
				* status = MTAPI_ERR_ATTR_SIZE;
			} else  {
				memcpy (attribute, attributes->entries[attribute_num].attribute_d, attribute_size);
				* status = MTAPI_SUCCESS;
			}
			break;
		case MTAPI_DOMAIN_SHARED :
			if (attribute_size != sizeof(mtapi_uint_t)) {
				* status = MTAPI_ERR_ATTR_SIZE;
			} else  {
				memcpy (attribute, attributes->entries[attribute_num].attribute_d, attribute_size);
				* status = MTAPI_SUCCESS;
			}
			break;
		default:
			*status = MTAPI_ERR_ATTR_NUM;
			break;
	}
}
void mtapiRT_initTaskAttribute (
	MTAPI_OUT mtapi_task_attributes_t * attributes,
	MTAPI_OUT mtapi_status_t * status
	)
{
	if (mtapiRT_nodeInitialized(status)) {
		memset(attributes, 0, sizeof(mtapi_task_attributes_t));
		*status = MTAPI_SUCCESS;
	} else {
		* status = MTAPI_ERR_NODE_NOTINIT;
	}
}

void mtapiRT_setTaskAttribute (
	MTAPI_INOUT mtapi_task_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_IN void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	switch (attribute_num) {
		case MTAPI_TASK_DETACHED :
			if (attribute_size != sizeof(mtapi_boolean_t)) {
				printf("attribute size dont match\n");
				* status = MTAPI_ERR_ATTR_SIZE;
				return; 
			}
			attributes->entries[attribute_num].valid = MTAPI_TRUE;
			attributes->entries[attribute_num].attribute_num = attribute_num;
			attributes->entries[attribute_num].bytes = attribute_size;
			attributes->entries[attribute_num].attribute_d = malloc (attribute_size);
			memcpy(attributes->entries[attribute_num].attribute_d, attribute, attribute_size);
			* status = MTAPI_SUCCESS;
			break;

		case MTAPI_TASK_INSTANCES :
			if (attribute_size != sizeof(mtapi_uint_t)) {
				printf("attribute size dont match\n");
				* status = MTAPI_ERR_ATTR_SIZE;
				return; 
			}
			attributes->entries[attribute_num].valid = MTAPI_TRUE;
			attributes->entries[attribute_num].attribute_num = attribute_num;
			attributes->entries[attribute_num].bytes = attribute_size;
			attributes->entries[attribute_num].attribute_d = malloc (attribute_size);
			memcpy(attributes->entries[attribute_num].attribute_d, attribute, attribute_size);
			* status = MTAPI_SUCCESS;
			break;

		default:
			*status =  MTAPI_ERR_ATTR_NUM;
			break;
	}
}

void mtapiRT_getTaskAttribute (
	MTAPI_IN mtapi_task_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_OUT void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	switch (attribute_num) {
		case MTAPI_TASK_DETACHED :
			if (attribute_size != sizeof(mtapi_boolean_t)) {
				* status = MTAPI_ERR_ATTR_SIZE;
			} else  {
				memcpy (attribute, attributes->entries[attribute_num].attribute_d, attribute_size);
				* status = MTAPI_SUCCESS;
			}
			break;
		case MTAPI_TASK_INSTANCES :
			if (attribute_size != sizeof(mtapi_uint_t)) {
				* status = MTAPI_ERR_ATTR_SIZE;
			} else  {
				memcpy (attribute, attributes->entries[attribute_num].attribute_d, attribute_size);
				* status = MTAPI_SUCCESS;
			}
			break;
		default:
			*status = MTAPI_ERR_ATTR_NUM;
			break;
	}
}

void mtapiRT_initAffinity (
	MTAPI_OUT mtapi_affinity_t * mask,
	MTAPI_IN mtapi_boolean_t affinity,
	MTAPI_OUT mtapi_status_t * status
		)
{
	int i;
	int m_cur = 0x0001;
	if (affinity) {
		for (i = 0; i < NUM_THREAD; i++) {
			m_bit = m_bit | m_cur;
			m_cur = 1 << m_cur;
		}
	}
	* status = MTAPI_SUCCESS;
}

void mtapiRT_setAffinity (
	MTAPI_INOUT mtapi_affinity_t * mask,
	MTAPI_IN mtapi_uint_t core_num,
	MTAPI_IN mtapi_boolean_t affinity,
	MTAPI_OUT mtapi_status_t * status
	)
{
	int m_cur = 0x0001;
	int i;
	if (!(core_num < NUM_THREAD) || core_num < 0) {
		* status = MTAPI_ERR_CORE_NUM;
		return;
	} else 
	if (affinity) {
		for (i = 0; i < core_num; i++) 
			m_cur = 1 << m_cur;
		m_bit = m_cur | m_bit;
	} else {
		for (i = 0; i < core_num; i++) 
			m_cur = 1 << m_cur;
		m_cur = ~m_cur;
		m_bit = m_bit & m_cur;
	}
	* status = MTAPI_SUCCESS;
}

mtapi_boolean_t mtapiRT_getAffinity (
	MTAPI_IN mtapi_affinity_t * mask,
	MTAPI_IN mtapi_uint_t core_num,
	MTAPI_OUT mtapi_status_t * status
	)
{
	int m_cur = 0x0001;
	int i;
	if (!(core_num < NUM_THREAD) || core_num < 0) {
		* status = MTAPI_ERR_CORE_NUM;
		return MTAPI_FALSE;
	} else {
		for (i = 0; i < core_num; i++) 
			m_cur = 1 << m_cur;
		m_cur = ~m_cur;
		if (m_cur & m_bit) 
			return MTAPI_TRUE;
		else 
			return MTAPI_FALSE;
	}
}
// End copy

int InsertTask (mtapiRT_TaskInfo_t * task) {
	if (!task) {
		return 1;
	}
	if (taskListHead == NULL) {
		taskListHead = task;
		taskListTail = task; 
	} else {
		taskListTail->next = task;
		taskListTail = task;
	}
	return 0;
}

mtapiRT_TaskInfo_t * GetTaskFromHead () {
	if (taskListHead == NULL) return NULL;
	else if (taskListHead == taskListTail) {
		mtapiRT_TaskInfo_t * task = taskListHead;
		taskListHead = NULL;
		taskListTail = NULL;
		return task;
	} else {
		mtapiRT_TaskInfo_t * task = taskListHead;
		taskListHead = taskListHead->next;
		return task;
	}
}

int InsertMessage(mtapiRT_Message_t * message) {
	if (message ==  NULL) {
		return 1;
	} 
	mtapiRT_MessageEntry_t * messageEntry = (mtapiRT_MessageEntry_t *) malloc (sizeof(mtapiRT_MessageEntry_t));
	messageEntry->message = message;
	messageEntry->next = NULL;
	if (messageListHead == NULL) {
		messageListHead = messageEntry;
		messageListTail = messageEntry;
	} else {
		messageListTail->next = messageEntry;
		messageListTail = messageEntry;
	}
	return 0;
}

int InsertMessageMaxPriority (mtapiRT_Message_t * message) {
	if (message == NULL) {
		return 1;
	}
	mtapiRT_MessageEntry_t * messageEntry = (mtapiRT_MessageEntry_t *) malloc (sizeof(mtapiRT_MessageEntry_t));
	messageEntry->message = message;
	messageEntry->next = NULL;
	if (messageListHead == NULL) {
		messageListHead = messageEntry;
		messageListTail = messageEntry;
	} else {
		messageEntry->next = messageListHead;
		messageListHead = messageEntry;
	}
	return 0;
}

mtapiRT_Message_t * GetMessageFromHead() {
	if (messageListHead == NULL) return NULL;
	else if (messageListHead == messageListTail) {
		mtapiRT_Message_t * message = messageListHead->message;
		mtapiRT_MessageEntry_t * tmp = messageListHead;
		messageListHead = NULL;
		messageListTail = NULL;
		free(tmp);
		return message;
	} else {
		mtapiRT_Message_t * message = messageListHead->message;
		mtapiRT_MessageEntry_t * tmp = messageListHead;
		messageListHead = messageListHead->next;
		free (tmp);
		return message;
	}
}

void ProcessTask(mtapiRT_TaskInfo_t * task) {
	if (task == NULL) {
		currentTask = NULL;
		return;
	}
	switch(task->context->status) {
		case MTAPI_TASK_CREATED:
			// If the task has a queue option. 
			if (task->queue != NULL) {
				pthread_mutex_lock(&task->queue->mutex);
				if (task != task->queue->head->task) { 
					// current task is not the head of queue.
					pthread_mutex_unlock(&task->queue->mutex);	
					// insert the task back to task list
					pthread_mutex_lock(&taskListMutex);
					InsertTask(task);
					pthread_mutex_unlock(&taskListMutex);
					break;
				} else {
					pthread_mutex_unlock(&task->queue->mutex);
				}
			}
			if (task->action->type ==  MTAPIRT_ACTION_LOCAL) {
				task->context->status = MTAPI_TASK_RUNNING;
				currentTask = task;
				task->action->location.fptr ( task->arguments,
	             				task->arguments_size,			     			     
					     	task->result,
				     	     	task->result_size,
				             	NULL,
				     	     	0, 
					     	NULL);
			// IMPORTANT: Must reset NULL when finished.
				currentTask = NULL;
				if (__sync_bool_compare_and_swap(&task->childCount, 0, 0)) {
					SetTaskComplete(task);
				} else {
					task->context->status = MTAPI_TASK_WAITING;
					pthread_mutex_lock(&taskListMutex);
					InsertTask(task);
					pthread_mutex_unlock(&taskListMutex);
				}
			} else { // create a message, ready to send.
				// TASK_SCHEDULED
				
				mtapiRT_RemoteTaskInfo_t * remoteTaskInfo = 
				(mtapiRT_RemoteTaskInfo_t *) malloc (
				sizeof(mtapiRT_RemoteTaskInfo_t));
				remoteTaskInfo->jobID = task->action->jobID;
				remoteTaskInfo->arguments_size = task->arguments_size;
				remoteTaskInfo->resultBuffer = NULL;
				remoteTaskInfo->resultSize = task->result_size;
				remoteTaskInfo->task = task;
				mtapiRT_Message_t * message = 
				(mtapiRT_Message_t *) malloc (
				sizeof(mtapiRT_Message_t));
				pthread_mutex_lock(&messageListMutex);
				message->type = MTAPIRT_MSG_START_REMOTE_TASK;
				message->senderDomain = _domainID;
				message->senderNode = _nodeID;
				// TODO seperate sender and receiver port
				message->senderPort = PORTID;
				message->buffer = (void *) remoteTaskInfo;
				message->bufferSize = sizeof(mtapiRT_RemoteTaskInfo_t);	
			//	task->context->status = MTAPI_TASK_SCHEDULED;
				// insert the message into message list
		//		pthread_mutex_lock(&messageListMutex);
		//		message->type = MTAPIRT_MSG_START_REMOTE_TASK;
				InsertMessage(message);
				pthread_mutex_unlock(&messageListMutex);
				task->context->status = MTAPI_TASK_SCHEDULED;
				pthread_mutex_lock(&taskListMutex);
				InsertTask(task);
				pthread_mutex_unlock(&taskListMutex);
 
				
			}
			break;
		case MTAPI_TASK_WAITING:
			// TODO compare and swap
//			if (task->childCount == 0) {
			if (__sync_bool_compare_and_swap(&task->childCount, 0, 0)) {
				SetTaskComplete(task);
			} else {
				pthread_mutex_lock(&taskListMutex);
				InsertTask(task);
				pthread_mutex_unlock(&taskListMutex);
			}
			break;
		case MTAPI_TASK_SCHEDULED:
			// if scheduled. running on remote node. continue;
			pthread_mutex_lock(&taskListMutex);
			InsertTask(task);
			pthread_mutex_unlock(&taskListMutex);
			break;
		default:
			printf("default\n");
			break;
	}
}

void ProcessMessage(mtapiRT_Message_t * message, mcapi_endpoint_t * localEndpoint) { 
	if (message == NULL) {
		return;
	}
	switch (message->type) {
		case MTAPIRT_MSG_START_REMOTE_TASK:	// send a signal to specific node.
			StartRemoteTask(message, localEndpoint);
			break;
		case MTAPIRT_MSG_RECEIVE_REMOTE_TASK:	// decode the message. Insert it into task list.
			ReceiveRemoteTask(message, localEndpoint);
			break;
		case MTAPIRT_MSG_AWAIT_REMOTE_TASK:	// check if the task has received from remote node.
			break;
		case MTAPIRT_MSG_AWAIT_LOCAL_TASK:
			AwaitLocalTask(message, localEndpoint);
			break;
		case MTAPIRT_MSG_COMPLETE_REMOTE_TASK:	// the remote task has been finished. Ready to send back the result.
			CompleteRemoteTask(message, localEndpoint);
			break;
		case MTAPIRT_MSG_COMPLETE_LOCAL_TASK:	// the remote task has been finished. Ready to send back the result.
			CompleteLocalTask(message, localEndpoint);
			break;
		case MTAPIRT_MSG_COMPLETE_TASK_WAIT:	// received the result from remote node. Delete the message entry.
			break;
		case MTAPIRT_MSG_CREATE_ACTION:
			CreateAction(message, localEndpoint);
			break;
		case MTAPIRT_MSG_ADD_REMOTE_ACTION:
			AddRemoteAction(message, localEndpoint);
			break;
		default:
			break;
	}
}

void CreateAction(mtapiRT_Message_t * message, mcapi_endpoint_t * sendEndpoint){
	int i;
	unsigned bufferSize;
	unsigned sendSize;
	size_t messageSize;
	char buffer[MCAPI_MAX_MSG_SIZE];
        mcapi_status_t status;
	mcapi_endpoint_t remoteEndpoint;
	for (i = 0; i < NODE_COUNT; i++) {
		
		if (_domainID == nodeList[i].domainID && 
		    _nodeID == nodeList[i].nodeID) {
			continue;
		}
		remoteEndpoint = mcapi_endpoint_get (
				nodeList[i].domainID,
				nodeList[i].nodeID+10,
				PORTID+1,
				MCAPI_TIMEOUT_INFINITE,
				&status
				);
		message->senderDomain = _domainID;
		message->senderNode = _nodeID;
		message->senderPort = PORTID;
		bufferSize = MarshallMessage (message, buffer);
		mcapi_msg_send (*sendEndpoint,
			       	remoteEndpoint, 
				&buffer, 
				bufferSize, 
				MCAPI_MAX_PRIORITY, 
				&status);
	}
		
}

void AddRemoteAction(mtapiRT_Message_t * message, mcapi_endpoint_t * sendEndpoint) {
	mtapi_job_id_t jobID = *(mtapi_job_id_t *) message->buffer;
//	printf("ADD REMOTE ACTION: %d %d %d\n", message->senderDomain, message->senderNode, jobID);
	mtapiRT_createActionDummy(message->senderDomain, message->senderNode, jobID);	
}
void StartRemoteTask (mtapiRT_Message_t * message, mcapi_endpoint_t * sendEndpoint) {
	// this message is created when task_start.
	// - send to the specific node. The message should have a type of 
	// ReceiveRemoteTask?
	// - switch the state to MTAPIRT_MSG_AWAIT_REMOTE_TASK (NO NEED TO PUT BACK)
	// - insert the message back to the message list. (NO NEED TO PUT BACK)
	unsigned bufferSize;
	unsigned sendSize;
	size_t messageSize;
	char buffer[MCAPI_MAX_MSG_SIZE];	
	mcapi_status_t status;
	mcapi_endpoint_t remoteEndpoint;
	mtapiRT_RemoteTaskInfo_t * remoteTask = (mtapiRT_RemoteTaskInfo_t *) message->buffer;
	mtapiRT_TaskInfo_t * task = remoteTask->task;
//	if (task->action->location.remoteNode.domain == 1) 
//		task->action->location.remoteNode.domain = 0;
	if (_domainID == 1 && _nodeID == 0)
	{
	remoteEndpoint = mcapi_endpoint_get (
			0,
			10,
			1,
			MCAPI_TIMEOUT_INFINITE,
			&status
			);
	} else {
	remoteEndpoint = mcapi_endpoint_get (
			task->action->location.remoteNode.domain,
			task->action->location.remoteNode.node+9,
			task->action->location.remoteNode.port + 1,
			MCAPI_TIMEOUT_INFINITE,
			&status
			);
	}	
	message->senderDomain = _domainID;
	message->senderNode = _nodeID;
	message->senderPort = PORTID;
	/*
	pthread_mutex_lock(&messageListMutex);
	message->type = MTAPIRT_MSG_RECEIVE_REMOTE_TASK;
	pthread_mutex_unlock(&messageListMutex);*/
	bufferSize = MarshallMessage (message, buffer);
	memcpy(&buffer[bufferSize], task->arguments, task->arguments_size);
	sendSize = bufferSize + task->arguments_size;
	/*
	remoteEndpoint = mcapi_endpoint_get (
			task->action->location.remoteNode.domain,
			task->action->location.remoteNode.node+10,
			task->action->location.remoteNode.port + 1,
			MCAPI_TIMEOUT_INFINITE,
			&status
			);
			*/
	mcapi_msg_send (*sendEndpoint,
		       	remoteEndpoint, 
			&buffer, 
			sendSize, 
			MCAPI_MAX_PRIORITY, 
			&status);
	mcapi_msg_recv(*sendEndpoint, &buffer, MCAPI_MAX_MSG_SIZE, &messageSize, &status);
//	UnmarshallMessage (buffer, messageSize, message);

	
//	printf("send task %d\n",*(int *) task->arguments);
	sendCnt++;
//	printf("sendCnt = %d\n", sendCnt);
	// TODO free this message;
}

void ReceiveRemoteTask(mtapiRT_Message_t * message, mcapi_endpoint_t * sendEndpoint) {
	// - decode the message
	// - create a task and insert it into the message list.
	// - switch the message into MTAPIRT_MSG_AWAIT_LOCAL_TASK. 
	// - insert the message back into message list. (NO NEED TO).
	mtapi_boolean_t dummyBool;
	mtapiRT_RemoteTaskInfo_t * remoteTask = (mtapiRT_RemoteTaskInfo_t *) message->buffer;
	void * arguments = malloc (remoteTask->arguments_size);
	remoteTask->resultBuffer = malloc (remoteTask->resultSize);
	memcpy(arguments, &message->buffer[sizeof(mtapiRT_RemoteTaskInfo_t)], remoteTask->arguments_size);
//	printf("start task %d\n", *(int *) arguments);
	mtapiRT_TaskInfo_t * task = mtapiRT_startTask (
			0, // dummy task id
			mtapiRT_getJob(remoteTask->jobID),
			arguments,
			remoteTask->arguments_size,
			remoteTask->resultBuffer,
			remoteTask->resultSize,
			NULL,
//			dummyBool,
	//		NULL,
			message	
			);
	pthread_mutex_lock(&messageListMutex);
	message->type = MTAPIRT_MSG_AWAIT_LOCAL_TASK;
	InsertMessage(message);
	pthread_mutex_unlock(&messageListMutex);
	awaitCnt++;
//	printf("start local task %d\n", *(int *) arguments);
}

void AwaitLocalTask(mtapiRT_Message_t * message, mcapi_endpoint_t * sendEndpoint) {
	// - insert back into message list.	(NO NEED TO);
	// - 
	pthread_mutex_lock(&messageListMutex);
	InsertMessage(message);
	pthread_mutex_unlock(&messageListMutex);
}	

void AwaitRemoteTask(mtapiRT_Message_t * message, mcapi_endpoint_t * sendEndpoint) {
	// - insert back into message list.  (NO NEED)
}

void CompleteLocalTask(mtapiRT_Message_t * message, mcapi_endpoint_t * sendEndpoint) {
	// pack the message and send it back where it comes from.
	unsigned bufferSize;
	unsigned sendSize;
	size_t messageSize;
	char buffer[MCAPI_MAX_MSG_SIZE];	
	mcapi_status_t status;
	mcapi_endpoint_t remoteEndpoint;
	remoteEndpoint = mcapi_endpoint_get (
			message->senderDomain,
			message->senderNode+10,
			message->senderPort + 1,
			MCAPI_TIMEOUT_INFINITE,
			&status
			);
	completeLocalCnt++;
//	printf("completeLocalCnt %d\n", completeLocalCnt);
	mtapiRT_RemoteTaskInfo_t * remoteTask = (mtapiRT_RemoteTaskInfo_t *) message->buffer;
	mtapiRT_TaskInfo_t * task = remoteTask->task;
	message->senderDomain = _domainID;
	message->senderNode = _nodeID;
	/*
	pthread_mutex_lock(&messageListMutex);
	message->type = MTAPIRT_MSG_COMPLETE_REMOTE_TASK;
	pthread_mutex_unlock(&messageListMutex);
	*/
	message->buffer = remoteTask->resultBuffer;
	message->bufferSize = remoteTask->resultSize;
	bufferSize = MarshallMessage (message, buffer);
	// TODO task should be &task
	memcpy(&buffer[bufferSize], &task, sizeof(mtapiRT_TaskInfo_t *));
	sendSize = bufferSize + sizeof(mtapiRT_TaskInfo_t *);
	/*
	remoteEndpoint = mcapi_endpoint_get (
			message->senderDomain,
			message->senderNode+10,
			message->senderPort + 1,
			MCAPI_TIMEOUT_INFINITE,
			&status
			);
			*/
	mcapi_msg_send (*sendEndpoint,
		       	remoteEndpoint, 
			&buffer, 
			sendSize, 
			MCAPI_MAX_PRIORITY, 
			&status);
//	printf("Complete Local task %d\n", *(int *) remoteTask->resultBuffer);
	mcapi_msg_recv(*sendEndpoint, &buffer, MCAPI_MAX_MSG_SIZE, &messageSize, &status);
//	UnmarshallMessage (buffer, messageSize, message);
	// TODO free the message
	
}

void CompleteRemoteTask(mtapiRT_Message_t * message, mcapi_endpoint_t * sendEndpoint) {
	// copy the result buffer back to the task's result buffer. 
	// SetTaskComplete. TODO verify if we can use SetTaskComplete due to 
	// the task may have a message attached.
	//
	mtapiRT_TaskInfo_t * task;	
	memcpy(&task, &message->buffer[message->bufferSize -  sizeof(mtapiRT_TaskInfo_t *)], sizeof(mtapiRT_TaskInfo_t *));
//	SetTaskComplete(task);
	memcpy(task->result, &message->buffer[0], message->bufferSize - sizeof(mtapiRT_TaskInfo_t *));
	task->context->status = MTAPI_TASK_WAITING;
          
}
void CompleteTaskWait(mtapiRT_Message_t * message, mcapi_endpoint_t * sendEndpoint) {
	// - decode the message.
	// - copy the result.
	// - remove the message and free the memory. 
}



void SetTaskComplete(mtapiRT_TaskInfo_t * task) {
	task->context->status = MTAPI_TASK_COMPLETED;
	if (task->parent) {
		__sync_fetch_and_add(&task->parent->childCount, -1);
	}
	// queue clean-up
	if (task->queue) {
		pthread_mutex_lock(&task->queue->mutex);
		if (task->queue->head == task->queue->tail) {
			// head == tail. One task entry in queue.
			mtapiRT_TaskEntry_t * currentEntry = task->queue->head;
			task->queue->head = NULL;
			task->queue->tail = NULL;
			free(currentEntry);
		} else {
			mtapiRT_TaskEntry_t * currentEntry = task->queue->head;
			task->queue->head = task->queue->head->next;
			free(currentEntry);
		}
		pthread_mutex_unlock(&task->queue->mutex);
	}
	// group clean-up
	if (task->group) {
		__sync_fetch_and_add (&task->group->currentNumOfTask, -1);
		// address of result
		task->group->result = task->result;
	}
	// change the status of the associated message to 
	// MTAPIRT_MSG_COMPLETE_LOCAL_TASK 
	if (task->message) {
		// MUST LOCK HERE 
		pthread_mutex_lock(&messageListMutex);
//		printf("task->message->type %d   \n", task->message->type);
		task->message->type = MTAPIRT_MSG_COMPLETE_LOCAL_TASK;
		pthread_mutex_unlock(&messageListMutex);
//		printf("waitCnt %d\n", ++waitCnt);
	}
//	printf("Finish task %d\n", *(int *) task->arguments);

}

void * thread_func (void * arg) {
	mtapiRT_TaskInfo_t * task;
	while (1) {
//		if (taskListHead == NULL) continue;
		pthread_mutex_lock(&taskListMutex);
		task = GetTaskFromHead() ;
		pthread_mutex_unlock(&taskListMutex);
		if (task) {
			ProcessTask(task);
		} else {
			continue;
		}	
	}
}

void * processer_func (void * args) {
	mcapi_status_t status;
	mcapi_endpoint_t localEndpoint;
	mcapi_param_t params;
	mcapi_info_t version;
	mcapi_port_t port = 0;
	mcapi_initialize(_domainID, _nodeID, NULL, &params, &version, &status);	
	localEndpoint = mcapi_endpoint_create(port, &status);
	while (1) {

		if (messageListHead == NULL) {
			// TODO verify if this step causes dead loops.
			sched_yield();
			continue;
		} else {
			pthread_mutex_lock(&messageListMutex);
			// retrieve one message and process.
			mtapiRT_Message_t * message = GetMessageFromHead();
//			ProcessMessage(message, &localEndpoint);
			pthread_mutex_unlock(&messageListMutex);
//			printf("%d\n", message->type);
			ProcessMessage(message, &localEndpoint);
		}
	}
}

void * listener_func(void * args) {

	mtapiRT_NodeInfo_t * nodeInfo = (mtapiRT_NodeInfo_t *) args;
//	mtapi_domain_t domainID = nodeInfo->domainID;
//	mtapi_node_t nodeID = nodeInfo->nodeID;

	// mcapi arguments
	mcapi_status_t status;
	mcapi_endpoint_t localEndpoint, remoteEndpoint;
	mcapi_param_t params;
	mcapi_info_t version;
	mcapi_port_t port = 0;
	mcapi_uint_t nrMessage;

	char buffer[MCAPI_MAX_MSG_SIZE];
	size_t messageSize, bufferSize;

	mcapi_initialize(_domainID, _nodeID+10, NULL, &params, &version, &status);
	localEndpoint = mcapi_endpoint_create(port, &status);
	mcapi_endpoint_t recvEndpoint = mcapi_endpoint_create (port + 1, &status);
	// sending/receiving request
	while (1) {
		// TODO check if received any messages. If any. pack and push into the message list.
		// Need to lock the messageMutex before insert into messageList.
		
		nrMessage = mcapi_msg_available (recvEndpoint, &status);
		// receive a message. Decode it and insert it into the message list.
		if (nrMessage) {
			mtapiRT_Message_t * message = (mtapiRT_Message_t *) malloc (sizeof(mtapiRT_Message_t));
			mcapi_msg_recv(recvEndpoint, &buffer, MCAPI_MAX_MSG_SIZE, &messageSize, &status);
			UnmarshallMessage (buffer, messageSize, message);
			pthread_mutex_lock(&messageListMutex);
			if (message->type == MTAPIRT_MSG_START_REMOTE_TASK) {
				message->type = MTAPIRT_MSG_RECEIVE_REMOTE_TASK;
			}
			if (message->type == MTAPIRT_MSG_COMPLETE_LOCAL_TASK) {
				message->type = MTAPIRT_MSG_COMPLETE_REMOTE_TASK;
			}
			if (message->type == MTAPIRT_MSG_CREATE_ACTION) {
				message->type = MTAPIRT_MSG_ADD_REMOTE_ACTION;
			}
			// test message received.
			if (message->type == MTAPIRT_MSG_RECEIVE_REMOTE_TASK ||
			    message->type == MTAPIRT_MSG_COMPLETE_REMOTE_TASK) {
				remoteEndpoint = mcapi_endpoint_get (
						 message->senderDomain,
						 message->senderNode,
					 	message->senderPort,
			 		 	MCAPI_TIMEOUT_INFINITE,
					 	&status
					 	);
				mtapiRT_Message_t * ack = (mtapiRT_Message_t *) malloc (sizeof(mtapiRT_Message_t));
				ack->type = MTAPIRT_MSG_ACK;
				ack->senderDomain = _domainID;
				ack->senderNode = _nodeID;
				ack->senderPort = PORTID;
				ack->buffer = NULL;
				ack->bufferSize = 0;
				bufferSize = MarshallMessage (ack, buffer);		
				mcapi_msg_send (localEndpoint,
		       				remoteEndpoint, 
						&buffer, 
						bufferSize, 
						MCAPI_MAX_PRIORITY, 
						&status);
			}
//			mtapiRT_RemoteTaskInfo_t * remoteTask = (mtapiRT_RemoteTaskInfo_t *) message->buffer;
//			printf("Receive %d\n", *(int *) &message->buffer[sizeof(mtapiRT_RemoteTaskInfo_t)]);

//			pthread_mutex_lock(&messageListMutex);
			InsertMessage(message);
			pthread_mutex_unlock(&messageListMutex);
//			recvCnt++;
//			printf("recvCnt is %d\n", recvCnt);			
			// TODO check if this works. This is a trick
			continue;
		}/*
		// The message list is empty. continue;
		if (messageListHead == NULL) {
			// TODO verify if this step causes dead loops.
//			sched_yield();
			continue;
		} else {
			pthread_mutex_lock(&messageListMutex);
			// retrieve one message and process.
			mtapiRT_Message_t * message = GetMessageFromHead();
			pthread_mutex_unlock(&messageListMutex);
			ProcessMessage(message, &localEndpoint);
		}
		*/
	}
}




unsigned MarshallMessage(mtapiRT_Message_t * message, char * buffer) {
	unsigned i, messageTypeSize, senderDomainSize, senderNodeSize, senderPortSize;
	messageTypeSize = sizeof(message->type);
	senderDomainSize = sizeof(message->senderDomain);
	senderNodeSize = sizeof(message->senderNode);
	senderPortSize = sizeof(message->senderPort);

	memcpy(&buffer[0], &message->type, messageTypeSize);
	memcpy(&buffer[messageTypeSize], &message->senderDomain, senderDomainSize);
	memcpy(&buffer[messageTypeSize + senderDomainSize], &message->senderNode, senderNodeSize);
	memcpy(&buffer[messageTypeSize + senderDomainSize + senderNodeSize], 
	       &message->senderPort, senderPortSize);
	for (i = 0; i < message->bufferSize; i++) {
		buffer[i + messageTypeSize + senderDomainSize + senderNodeSize + senderPortSize] = message->buffer[i];
	}	
	return messageTypeSize + senderDomainSize + senderNodeSize + senderPortSize + message->bufferSize;

}

mtapi_boolean_t UnmarshallMessage(char * buffer, unsigned bufferSize, mtapiRT_Message_t * message) {
	unsigned  messageTypeSize, senderDomainSize, senderNodeSize, senderPortSize;
	messageTypeSize = sizeof(message->type);
	senderDomainSize = sizeof(message->senderDomain);
	senderNodeSize = sizeof(message->senderNode);
	senderPortSize = sizeof(message->senderPort);
	message->bufferSize = bufferSize - (messageTypeSize + senderDomainSize + senderNodeSize + senderPortSize);

	memcpy(&message->type, &buffer[0], messageTypeSize);
	memcpy(&message->senderDomain, &buffer[messageTypeSize], senderDomainSize);
	memcpy(&message->senderNode, &buffer[messageTypeSize + senderDomainSize], senderNodeSize);
	memcpy(&message->senderPort, &buffer[messageTypeSize + senderDomainSize + senderNodeSize], senderPortSize);
	char * newBuffer = (char *) malloc (message->bufferSize);
	memcpy(newBuffer, &buffer[messageTypeSize + senderDomainSize + senderNodeSize + senderPortSize], message->bufferSize);
	message->buffer = newBuffer;
	return MTAPI_TRUE;
}


















