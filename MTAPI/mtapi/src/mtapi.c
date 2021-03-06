#include "mtapi.h"
#include "mtapi_runtime.h"
#include <stdio.h>
#include <stdlib.h>

mtapi_domain_t mtapi_domain_id_get ( mtapi_status_t * status)
{
}

mtapi_node_t mtapi_node_id_get (mtapi_status_t * status)
{
}
/**
 * This function initialize the MTAPI environment on a given MTAPI node in 
 * a given MTAPI domain. It must be called on each node using MTAPI. A node
 * maps to a process, thread, thread pool, instance of an operating system,
 * hardware accelerator, processor core, a cluster of processor cores, or 
 * other abstract processing entity with an independent program counter. In
 * other words, an MTAPI node is an indepedent thread of control.
 *
 * Application software running on an MTAPI node must call mtapi_initialize
 * once per node. It is an error to call mtapi_initialize multiple times 
 * from a given node, unless mtapi_finalize is called in between.
 *
 * The values for domain_id and node_id must be known a priority by the 
 * application and MTAPI.
 *
 * mtapi_info is used to obtain information from the MTAPI implementation, 
 * includeing MTAPI and the underlying implementation version numbers, 
 * implementation vendor identification, the number fo cores ofa node, 
 * and vendor-specific implemenation information.
 *
 * A given MTAPI implementation will specify what is a node, i.e how the 
 * concrete system is partitioned into nodes and what are teh underlying 
 * units of execution tasks. e.g threads, a thread pool, processes, or 
 * hardware units.
 *
 * attributes is a pointer to a node attributes object that was previously
 * prepared with mtapi_nodeattr_init and mtapi_nodeattr_set. If attribute
 * is MTAPI_NULL, then implementation-defined default attributes will be
 * used.
 */
void mtapi_initialize (
   	MTAPI_IN mtapi_domain_t domain_id,
	MTAPI_IN mtapi_node_t node_id,
	MTAPI_IN mtapi_node_attributes_t* mtapi_node_attributes,
	MTAPI_OUT mtapi_info_t* mtapi_info,
	MTAPI_OUT mtapi_status_t* status
	)
{
	mtapiRT_initialize(domain_id, node_id);
}

/**
 * This function creates a software action. It is called on the node where
 * the action function is implemented. An action is an abstract 
 * encapsulation of everything needed to implement a job. An action contain
 * attributes, a reference to a job, a reference to an action function,
 * and a reference to node-local data. After an action is created, it is 
 * referenced by the application using a node-local handle of type 
 * mtapi_action_hndl_t. 
 */

mtapi_action_hndl_t mtapi_action_create(
	MTAPI_IN mtapi_job_id_t job_id,
	MTAPI_IN mtapi_action_function_t function,
	MTAPI_IN void * node_local_data,
	MTAPI_IN mtapi_size_t node_local_data_size,
	MTAPI_IN mtapi_action_attributes_t * attributes,
	MTAPI_OUT mtapi_status_t * status
)
{	
	mtapi_action_hndl_t action_hndl = {(void *) mtapiRT_createAction(job_id, function)};
	return action_hndl;
}

mtapi_action_hndl_t mtapi_action_create_dummy (
		mtapi_job_id_t job_id
) {
	mtapi_action_hndl_t action_hndl = {(void *) mtapiRT_createActionDummy(1, 1, job_id)};
	return action_hndl;
}	

/**
 * This fuction deletes a software action. (Hardware actions exist
 * perpetually and cannot be deleted.)
 *
 * mtapi_action_delete() may be called by any node that has a valid action
 * handle. Tasks associated with an action that has been deleted may still
 * be executed depending on internal state:
 *
 * 1. If mtai_action_delete() is called on an action that is currently 
 * executing, the associated task's state will be set to 
 * MTAPI_TASK_CANCELLED and execution will continue. To accomplish this, 
 * action functions must poll the task state with mtapi_context_taskstate_get. 
 * A call to mtapi_task_wait() on this task will return the status of 
 * mtapi_context_status_set. or MTAPI_SUCCESS if not explicitly set.
 *
 * 2. Tasks that are started or enqueued but waiting fro execution by the
 * MTAPI runtime wwhen mtapi_action_delete() is called will not be executed
 * any more if the deleted action is the only action associated with that
 * task. A call to mtapi_task_wait() will return MTAPI_ERR_ACTION_DELETED.
 *
 * 3. Tasks that are started or enqueued ater deletion of the action will
 * return MTAPI_ERR_ACTION_INVALID if the deleted acion is the only action
 * associated with the task.
 */
void mtapi_action_delete(
	MTAPI_IN mtapi_action_hndl_t actionHndl,
	MTAPI_IN mtapi_timeout_t timeout,
	MTAPI_OUT mtapi_status_t * status
		)
{	/*
	mtapiRT_ActionInfo_t * action = (mtapiRT_ActionInfo_t * ) actionHndl.descriptor;
	mtapiRT_deleteAction(action, timeout, status);*/
}

/**
 * This function disables an action. Tasks associated with an action that 
 * has been disabled may still be executed depending on their internal state.
 *
 * 1. If mtai_action_disable() is called on an action that is currently 
 * executing, the associated task's state will be set to 
 * MTAPI_TASK_CANCELLED and execution will continue. To accomplish this, 
 * action functions must poll the task state with mtapi_context_taskstate_get. 
 * A call to mtapi_task_wait() on this task will return the status of 
 * mtapi_context_status_set. or MTAPI_SUCCESS if not explicitly set.
 *
 * 2. Tasks that are started or enqueued but waiting fro execution by the
 * MTAPI runtime wwhen mtapi_action_delete() is called will not be executed
 * any more if the deleted action is the only action associated with that
 * task. A call to mtapi_task_wait() will return MTAPI_ERR_ACTION_DISABLED.
 *
 * 3. Tasks that are started or enqueued ater deletion of the action will
 * return MTAPI_ERR_ACTION_DISABLED if the deleted acion is the only action
 * associated with the task.
 */
 

void mtapi_action_disable(
	MTAPI_IN mtapi_action_hndl_t actionHndl,
	MTAPI_IN mtapi_timeout_t timeout,
	MTAPI_OUT mtapi_status_t * status
		)
{/*
	mtapiRT_ActionInfo_t * action = (mtapiRT_ActionInfo_t * ) actionHndl.descriptor;
	mtapiRT_disableAction(action, timeout, status);*/
}

/**
 * This function enables a previously disabled action. If this function is 
 * called on an action that no longer exists, an MTAPI_ERR_ACTION_INVALIDE
 * error will be returned.
 */

void mtapi_action_enable(
	MTAPI_IN mtapi_action_hndl_t actionHndl,
	MTAPI_IN mtapi_timeout_t timeout,
	MTAPI_OUT mtapi_status_t * status
		)
{/*
	mtapiRT_ActionInfo_t * action = (mtapiRT_ActionInfo_t * ) actionHndl.descriptor;
	mtapiRT_enableAction(action, timeout, status);*/
}

/**
 * Given a job_id, this function returns the MTAPI handle for referencing 
 * the actions implementing the job. This function converts a domain-wide
 * job ID into a node-local job handle.
 */

mtapi_job_hndl_t mtapi_job_get (
		MTAPI_IN mtapi_job_id_t job_id,
		MTAPI_OUT mtapi_status_t * status
		)
{
	mtapi_job_hndl_t jobHndl = {(void *) mtapiRT_getJob(job_id)};
	return jobHndl;
}

/**
 * This function schedules a task for execution. A task is associated with a
 * job. A job is associated with one or more actions. An action provides an
 * action function, which is the executable implementation of a job. If more
 * than one action is associated with the job, the runtime system will 
 * choose dynamically which action is used for execution for load balancing
 * purposes.
 */

mtapi_task_hndl_t mtapi_task_start (
		MTAPI_IN mtapi_task_id_t task_id,
		MTAPI_IN mtapi_job_hndl_t job,
		MTAPI_IN void * arguments,
		MTAPI_IN mtapi_size_t arguments_size,
		MTAPI_OUT void * result_buffer,
		MTAPI_IN mtapi_size_t result_size,
		MTAPI_IN mtapi_task_attributes_t * attributes,
		MTAPI_IN mtapi_group_hndl_t group,
		MTAPI_OUT mtapi_status_t * status
		)
{
	mtapiRT_JobInfo_t * jobPtr = (mtapiRT_JobInfo_t *) job.descriptor;
	mtapiRT_TaskInfo_t * taskPtr = mtapiRT_startTask ( 
			task_id,
			jobPtr,
			arguments,
			arguments_size,
			result_buffer,
			result_size,
			(mtapiRT_GroupInfo_t * ) group.descriptor,
	//		MTAPI_FALSE,
	//		MTAPI_NULL,
			MTAPI_NULL
			);
	mtapi_task_hndl_t task = {(void *) taskPtr};
	return task;
}

/**
 * This function scheduled a task for execution using a queue. A queue is
 * associated with a job. The tasks in the queue must be executed 
 * sequenctially.
 */

mtapi_task_hndl_t mtapi_task_enqueue(
		MTAPI_IN mtapi_task_id_t task_id,
		mtapi_queue_hndl_t queue,
		MTAPI_IN void * arguments,
		MTAPI_IN mtapi_size_t arguments_size,
		MTAPI_OUT void * result_buffer,
		MTAPI_IN mtapi_size_t result_size,
		MTAPI_IN mtapi_task_attributes_t * attributes,
		MTAPI_IN mtapi_group_hndl_t group,
		MTAPI_OUT mtapi_status_t * status
		)
{	
	mtapi_task_hndl_t taskHndl= {(void *) mtapiRT_enqueueTask (
			task_id,
			(mtapiRT_QueueInfo_t *) queue.descriptor, 
			arguments,
			arguments_size, 
			result_buffer,
			result_size,
			(mtapiRT_GroupInfo_t *) group.descriptor
			)};
	return taskHndl;
}

/**
 * This function waits for the completion of the specified task.
 *
 * taskHndl must be a valid handle obtained by a previously call to 
 * mtapi_task_start() or mtapi_task_enqueue(). The task_handle becomes 
 * invalid on a successful wait.
 *
 * timeout determines how long the function should wait for this task.
 *
 * Results of a completed task can be obtained via result_buffer associated
 * with the task. The size of the buffer has to be equal to the result size
 * written in the action code. 
 *
 * Calling mtapi_task_wait() more than once for the same task results in 
 * undefined behavior.
 */
void mtapi_task_wait (
		MTAPI_IN mtapi_task_hndl_t taskHndl,
		MTAPI_IN mtapi_timeout_t timeout,
		MTAPI_OUT mtapi_status_t *status
		)
{
	mtapiRT_TaskInfo_t *task = (mtapiRT_TaskInfo_t *) taskHndl.descriptor;
	mtapiRT_waitForTask(
			task
			);	
}

/**
 * This function creates a software queue object and associated it with the
 * specified job. Hardware queues are considered to be pre-existant and 
 * do not need to be created.
 *
 * queue_id is an identifier of implementation-defined type that must be 
 * supplied by the application. job is a handle to a job object.
 */
mtapi_queue_hndl_t mtapi_queue_create (
		MTAPI_IN mtapi_queue_id_t queue_id,
		MTAPI_IN mtapi_job_hndl_t job,
		MTAPI_IN mtapi_queue_attributes_t * attributes,
		MTAPI_OUT mtapi_status_t * status
		)
{
	mtapi_queue_hndl_t queueHndl = {(void *) mtapiRT_createQueue (queue_id, (mtapiRT_JobInfo_t *) job.descriptor)};
	return queueHndl;
}


void mtapi_nodeattr_init (
		MTAPI_OUT mtapi_node_attributes_t * attributes,
		MTAPI_OUT mtapi_status_t * status
		)
{
	*status = MTAPI_ERR_UNKNOWN;
	if (attributes == MTAPI_NULL) {
		*status = MTAPI_ERR_PARAMETER;
	} else {
		mtapiRT_initNodeAttribute(attributes, status);
	}
}

void mtapi_nodeattr_set (
	MTAPI_INOUT mtapi_node_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_IN void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	*status = MTAPI_ERR_UNKNOWN;
	if (attribute == MTAPI_NULL || attributes == NULL) {
		* status = MTAPI_ERR_PARAMETER;
	} else {
		mtapiRT_setNodeAttribute(
				attributes,
				attribute_num,
				attribute,
				attribute_size,
				status);
	}
}

void mtapi_node_get_attribute (
	MTAPI_IN mtapi_node_t node,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_OUT void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	*status = MTAPI_ERR_UNKNOWN;
	if (attribute == MTAPI_NULL) {
		* status = MTAPI_ERR_PARAMETER;
	} else {
		mtapiRT_getNodeAttribute (
				node,
				attribute_num,
				attribute,
				attribute_size,
				status
				);
	}
}



void mtapi_actionattr_init (
		MTAPI_OUT mtapi_action_attributes_t * attributes,
		MTAPI_OUT mtapi_status_t * status
		)
{
	*status = MTAPI_ERR_UNKNOWN;
	if (attributes == MTAPI_NULL) {
		*status = MTAPI_ERR_PARAMETER;
	} else {
		mtapiRT_initActionAttribute(attributes, status);
	}
}


void mtapi_actionattr_set (
	MTAPI_INOUT mtapi_action_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_IN void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	*status = MTAPI_ERR_UNKNOWN;
	if (!mtapiRT_nodeInitialized(status)) {
		* status = MTAPI_ERR_NODE_NOTINIT;
		return;
	} 
	if (attribute == MTAPI_NULL || attributes == NULL) {
		* status = MTAPI_ERR_PARAMETER;
	} else {
		mtapiRT_setActionAttribute(
				attributes,
				attribute_num,
				attribute,
				attribute_size,
				status);
	}
}

void mtapi_action_set_attribute (
	MTAPI_IN mtapi_action_hndl_t action,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_IN void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	mtapiRT_ActionInfo_t * actionInfo = (mtapiRT_ActionInfo_t * ) action.descriptor;
	if (actionInfo == MTAPI_NULL) {
		* status = MTAPI_ERR_ACTION_INVALID;
	} else
	if (attribute == MTAPI_NULL || actionInfo->attributes == MTAPI_NULL) { 
		* status = MTAPI_ERR_PARAMETER;
	} else {
 		mtapi_actionattr_set (
			actionInfo->attributes,
			attribute_num,
			attribute,
			attribute_size,
			status
				);
	}
}

void mtapi_action_get_attribute (
	MTAPI_IN mtapi_action_hndl_t action,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_OUT void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	*status = MTAPI_ERR_UNKNOWN;
	mtapiRT_ActionInfo_t * actionInfo = (mtapiRT_ActionInfo_t * ) action.descriptor;
	if (attribute == MTAPI_NULL || actionInfo->attributes == MTAPI_NULL) {
		* status = MTAPI_ERR_PARAMETER;
	} else {
		mtapiRT_getActionAttribute (
				actionInfo->attributes,
				attribute_num,
				attribute,
				attribute_size,
				status
				);
	}
}


void mtapi_queueattr_init (
		MTAPI_OUT mtapi_queue_attributes_t * attributes,
		MTAPI_OUT mtapi_status_t * status
		)
{
	*status = MTAPI_ERR_UNKNOWN;
	if (attributes == MTAPI_NULL) {
		*status = MTAPI_ERR_PARAMETER;
	} else {
		mtapiRT_initQueueAttribute(attributes, status);
	}
}

void mtapi_queueattr_set (
	MTAPI_INOUT mtapi_queue_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_IN void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	*status = MTAPI_ERR_UNKNOWN;
	if (!mtapiRT_nodeInitialized(status)) {
		* status = MTAPI_ERR_NODE_NOTINIT;
		return;
	} 
	if (attribute == MTAPI_NULL || attributes == NULL) {
		* status = MTAPI_ERR_PARAMETER;
	} else {
		mtapiRT_setQueueAttribute(
				attributes,
				attribute_num,
				attribute,
				attribute_size,
				status);
	}
}

void mtapi_queue_set_attribute (
	MTAPI_IN mtapi_queue_hndl_t queue,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_IN void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	mtapiRT_QueueInfo_t * queueInfo = (mtapiRT_ActionInfo_t * ) queue.descriptor;
	if (queueInfo == MTAPI_NULL) {
		* status = MTAPI_ERR_QUEUE_INVALID;
	} else
	if (attribute == MTAPI_NULL || queueInfo->attributes == MTAPI_NULL) { 
		* status = MTAPI_ERR_PARAMETER;
	} else {
 		mtapi_queueattr_set (
			queueInfo->attributes,
			attribute_num,
			attribute,
			attribute_size,
			status
				);
	}
}


void mtapi_queue_get_attribute (
	MTAPI_IN mtapi_queue_hndl_t queue,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_OUT void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	*status = MTAPI_ERR_UNKNOWN;
	mtapiRT_QueueInfo_t * queueInfo = (mtapiRT_QueueInfo_t * ) queue.descriptor;
	if (attribute == MTAPI_NULL || queueInfo->attributes == MTAPI_NULL) {
		* status = MTAPI_ERR_PARAMETER;
	} else {
		mtapiRT_getQueueAttribute (
				queueInfo->attributes,
				attribute_num,
				attribute,
				attribute_size,
				status
				);
	}
}



mtapi_group_hndl_t mtapi_group_create (
	MTAPI_IN mtapi_group_id_t group_id,
	MTAPI_IN mtapi_group_attributes_t * attributes,
	MTAPI_OUT mtapi_status_t * status
	)
{
	mtapi_group_hndl_t groupHndl = {(void *)mtapiRT_createGroup(group_id)};
	return groupHndl;
}

void mtapi_group_wait_all (
	MTAPI_IN mtapi_group_hndl_t group,
	MTAPI_IN mtapi_timeout_t timeout,
	MTAPI_OUT mtapi_status_t * status
	) 
{
	mtapiRT_waitForGroupAll((mtapiRT_GroupInfo_t *) group.descriptor);
}

void mtapi_group_wait_any (
	MTAPI_IN mtapi_group_hndl_t group,
	MTAPI_OUT void ** result,
	MTAPI_IN mtapi_timeout_t timeout,
	MTAPI_OUT mtapi_status_t * status
	)
{
	mtapiRT_waitForGroupAny ((mtapiRT_GroupInfo_t *) group.descriptor, result);
}

/*TODO fix the bug in context functions */

// Copy

void mtapi_taskattr_init (
		MTAPI_OUT mtapi_task_attributes_t * attributes,
		MTAPI_OUT mtapi_status_t * status
		)
{
	*status = MTAPI_ERR_UNKNOWN;
	if (attributes == MTAPI_NULL) {
		*status = MTAPI_ERR_PARAMETER;
	} else {
		mtapiRT_initTaskAttribute(attributes, status);
	}
}

void mtapi_taskattr_set (
	MTAPI_INOUT mtapi_task_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_IN void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	*status = MTAPI_ERR_UNKNOWN;
	if (!mtapiRT_nodeInitialized(status)) {
		* status = MTAPI_ERR_NODE_NOTINIT;
		return;
	} 
	if (attribute == MTAPI_NULL || attributes == NULL) {
		* status = MTAPI_ERR_PARAMETER;
	} else {
		mtapiRT_setTaskAttribute(
				attributes,
				attribute_num,
				attribute,
				attribute_size,
				status);
	}
}



void mtapi_task_get_attribute (
	MTAPI_IN mtapi_task_hndl_t task,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_OUT void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	*status = MTAPI_ERR_UNKNOWN;
	mtapiRT_TaskInfo_t * taskInfo = (mtapiRT_TaskInfo_t * ) task.descriptor;
	if (attribute == MTAPI_NULL || taskInfo->attributes == MTAPI_NULL) {
		* status = MTAPI_ERR_PARAMETER;
	} else {
		mtapiRT_getTaskAttribute (
				taskInfo->attributes,
				attribute_num,
				attribute,
				attribute_size,
				status
				);
	}
}

void mtapi_context_status_set (
	MTAPI_INOUT mtapi_task_context_t * task_context,
	MTAPI_IN mtapi_status_t error_code,
	MTAPI_OUT mtapi_status_t * status
	)
{
	if (!mtapiRT_nodeInitialized(status)) {
		* status = MTAPI_ERR_NODE_NOTINIT;
		return;
	}
	if (error_code != MTAPI_SUCCESS)
		task_context->status = MTAPI_TASK_CANCELLED;
	* status = error_code;
}

void mtapi_context_runtime_notify (
	MTAPI_INOUT mtapi_task_context_t * task_context,
	MTAPI_IN mtapi_notification_t notification,
	MTAPI_IN void * data,
	MTAPI_IN mtapi_size_t data_size,
	MTAPI_OUT mtapi_status_t * status
	)
{
	if (!mtapiRT_nodeInitialized(status)) {
		* status = MTAPI_ERR_NODE_NOTINIT;
	} else {
		task_context->notify = notification;
		task_context->data = malloc (data_size);
		memcpy(task_context->data, data, data_size);
		* status = MTAPI_SUCCESS;
	}
	
}


mtapi_task_state_t mtapi_context_taskstate_get (
	MTAPI_IN mtapi_task_context_t * task_context,
	MTAPI_OUT mtapi_status_t * status
	)
{
	if (!mtapiRT_nodeInitialized(status)) {
		* status = MTAPI_ERR_NODE_NOTINIT;
		return -1;
	} else {
		* status = MTAPI_SUCCESS;
		return task_context->status;
	}
}

mtapi_uint_t mtapi_context_instnum_get (
	MTAPI_IN mtapi_task_context_t * task_context,
	MTAPI_OUT mtapi_status_t * status
	)
{
	if(!mtapiRT_nodeInitialized(status)) {
		* status = MTAPI_ERR_NODE_NOTINIT;
		return -1;
	} else {
		* status = MTAPI_SUCCESS;
		return task_context->instance;
	}
}

mtapi_uint_t mtapi_context_numinst_get (
	MTAPI_IN mtapi_task_context_t * task_context,
	MTAPI_OUT mtapi_status_t * status
	)
{
	if(!mtapiRT_nodeInitialized(status)) {
		* status = MTAPI_ERR_NODE_NOTINIT;
		return -1;
	} else {
		* status = MTAPI_SUCCESS;
		return task_context->num_instances;
	}
}


mtapi_uint_t mtapi_context_corenum_get (
	MTAPI_IN mtapi_task_context_t * task_context,
	MTAPI_OUT mtapi_status_t * status
	)
{
	if(!mtapiRT_nodeInitialized(status)) {
		* status = MTAPI_ERR_NODE_NOTINIT;
		return -1;
	} else {
		* status = MTAPI_SUCCESS;
		return task_context->num_core;
	}
}

void mtapi_affinity_init (
	MTAPI_OUT mtapi_affinity_t * mask,
	MTAPI_IN mtapi_boolean_t affinity,
	MTAPI_OUT mtapi_status_t * status
	)
{
	if (mask == MTAPI_NULL) {
		*status = MTAPI_ERR_AFFINITY_MASK;
	} else 
	if (! mtapiRT_nodeInitialized(status)) {
		*status = MTAPI_ERR_NODE_NOTINIT;
	} else {
		mtapiRT_initAffinity (
				mask,
				affinity,
				status
			);
	}
}

void mtapi_affinity_set (
	MTAPI_INOUT mtapi_affinity_t * mask,
	MTAPI_IN mtapi_uint_t core_num,
	MTAPI_IN mtapi_boolean_t affinity,
	MTAPI_OUT mtapi_status_t * status
	)
{
	if (mask == MTAPI_NULL) {
		* status = MTAPI_ERR_AFFINITY_MASK;
	} else 
	if (! mtapiRT_nodeInitialized(status)) {
		* status = MTAPI_ERR_NODE_NOTINIT;
	} else {
		mtapiRT_setAffinity ( 
				mask,
				core_num,
				affinity,
				status
				);
	}
}

mtapi_boolean_t mtapi_affinity_get (
	MTAPI_IN mtapi_affinity_t * mask,
	MTAPI_IN mtapi_uint_t core_num,
	MTAPI_OUT mtapi_status_t * status
	)
{
	if (mask == MTAPI_NULL) {
		*status = MTAPI_ERR_AFFINITY_MASK;
		return MTAPI_FALSE;
	} else {
		return mtapiRT_getAffinity (
			mask,
			core_num,
			status
				);
	}
}
// End copy







