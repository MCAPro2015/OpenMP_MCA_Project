#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "mtapi.h"
#include "mcapi.h"

#define TLS __thread
#define NUM_JOB 100
#define NUM_NODE 32
#define NUM_THREAD 3
#define PORTID 0

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
extern const int NODE_INFOS[];
extern const int NODE_COUNT;
//extern const int NUM_THREAD;


typedef enum {
	MTAPIRT_MSG_ACK = 0,
	MTAPIRT_MSG_START_REMOTE_TASK, 
	MTAPIRT_MSG_RECEIVE_REMOTE_TASK,
	MTAPIRT_MSG_AWAIT_REMOTE_TASK,
	MTAPIRT_MSG_AWAIT_LOCAL_TASK,
	MTAPIRT_MSG_COMPLETE_REMOTE_TASK,
	MTAPIRT_MSG_COMPLETE_LOCAL_TASK,
	MTAPIRT_MSG_COMPLETE_TASK_WAIT,
	MTAPIRT_MSG_CREATE_ACTION,
	MTAPIRT_MSG_ADD_REMOTE_ACTION
} mtapiRT_MessageType_t;

typedef struct mtapiRT_NodeInfo{
	mtapi_domain_t domainID;
	mtapi_node_t nodeID;
} mtapiRT_NodeInfo_t;

typedef struct mtapiRT_SetInfo {
	mtapi_domain_t domainID;
	mtapi_node_t nodeID;
} mtapiRT_SetInfo_t;

typedef struct mtapiRT_Message {
	mtapiRT_MessageType_t type;
	mtapi_domain_t senderDomain;
	mtapi_node_t senderNode;
	mcapi_port_t senderPort;
	char * buffer;
	mtapi_uint_t bufferSize;
} mtapiRT_Message_t;

typedef struct mtapiRT_MessageEntry {
	struct mtapiRT_Message * message;
	struct mtapiRT_MessageEntry * next;
} mtapiRT_MessageEntry_t;
typedef enum {
	MTAPIRT_ACTION_LOCAL = 0,
	MTAPIRT_ACTION_REMOTE
} mtapiRT_ActionType_t;

typedef struct mtapiRT_LocationInfo {
	mtapi_domain_t domain;
	mtapi_node_t node;
	mcapi_port_t port;
} mtapiRT_LocationInfo_t;

typedef struct mtapiRT_RemoteTaskInfo {
	mtapi_job_id_t jobID;
	mtapi_size_t arguments_size;
	void * resultBuffer;
	mtapi_size_t resultSize;

	struct mtapiRT_TaskInfo * task;
} mtapiRT_RemoteTaskInfo_t;


typedef struct mtapiRT_ActionInfo {
	mtapi_job_id_t jobID;
	mtapiRT_ActionType_t type;
	union location_t {
		mtapi_action_function_t fptr;
		mtapiRT_LocationInfo_t remoteNode;
	} location;
	struct mtapiRT_ActionInfo * next;
	mtapi_action_attributes_t * attributes;
} mtapiRT_ActionInfo_t;

typedef struct mtapiRT_JobInfo {
	mtapiRT_ActionInfo_t * actionList;
} mtapiRT_JobInfo_t;

typedef struct mtapiRT_TaskInfo {
	mtapiRT_ActionInfo_t * action;
	mtapi_task_context_t * context;
	MTAPI_IN void * arguments;
	mtapi_size_t arguments_size;
	MTAPI_OUT void * result;
	mtapi_size_t result_size;
	struct mtapiRT_TaskInfo * next;

	struct mtapiRT_TaskInfo * parent;
	struct mtapiRT_LocationInfo * location;
	struct mtapiRT_Message * message;
	volatile unsigned childCount;

	struct mtapiRT_QueueInfo * queue;
	struct mtapiRT_GroupInfo * group;

	mtapi_task_attributes_t * attributes;
	
} mtapiRT_TaskInfo_t;

typedef struct mtapiRT_TaskEntry {
	mtapiRT_TaskInfo_t * task;
	struct mtapiRT_TaskEntry  * next;
} mtapiRT_TaskEntry_t;

typedef struct mtapiRT_QueueInfo {
	mtapi_queue_id_t id;
	mtapiRT_JobInfo_t * job;
	mtapiRT_TaskEntry_t * head;
	mtapiRT_TaskEntry_t * tail;
	pthread_mutex_t mutex;

	mtapi_queue_attributes_t * attributes;
} mtapiRT_QueueInfo_t;

typedef struct mtapiRT_GroupInfo {
	mtapi_group_id_t id;
	volatile unsigned numOfTask;
	volatile unsigned currentNumOfTask;
	void * result;
} mtapiRT_GroupInfo_t;


void mtapiRT_initialize(mtapi_domain_t, mtapi_node_t);


mtapiRT_JobInfo_t * mtapiRT_getJob (
		mtapi_job_id_t job_id
);

mtapiRT_ActionInfo_t * mtapiRT_createAction (
		mtapi_job_id_t job_id,
		mtapi_action_function_t function
);
mtapiRT_ActionInfo_t * mtapiRT_createActionDummy (
		mtapi_domain_t domainID,
		mtapi_node_t nodeID,
		mtapi_job_id_t job_id
);

mtapiRT_ActionInfo_t * mtapiRT_getAction (
		mtapiRT_JobInfo_t * job
);

mtapiRT_TaskInfo_t * mtapiRT_startTask (
		mtapi_task_id_t task_id,
		mtapiRT_JobInfo_t * job,
		MTAPI_IN void * arguments,
		MTAPI_IN size_t arguments_size,
		MTAPI_OUT void * result,
		MTAPI_IN size_t result_size,
		mtapiRT_GroupInfo_t * group,
//		mtapi_boolean_t isFromRemote,
//		mtapiRT_LocationInfo_t * location,
		mtapiRT_Message_t * message
);


void mtapiRT_waitForTask (
		mtapiRT_TaskInfo_t * task
);

mtapiRT_TaskInfo_t * mtapiRT_enqueueTask (
		mtapi_task_id_t task_id,
		mtapiRT_QueueInfo_t * queue,
		MTAPI_IN void * arguments,
		MTAPI_IN size_t arguments_size,
		MTAPI_OUT void * result,
		MTAPI_IN size_t result_size,
		mtapiRT_GroupInfo_t * group
);

mtapiRT_QueueInfo_t * mtapiRT_createQueue (
		mtapi_queue_id_t queue_id,
		mtapiRT_JobInfo_t * job
);

mtapiRT_GroupInfo_t * mtapiRT_createGroup (
		mtapi_group_id_t group_id
); 

void mtapiRT_waitForGroupAll (
		mtapiRT_GroupInfo_t * group
);

void mtapiRT_waitForGroupAny (
		mtapiRT_GroupInfo_t * group,
		void ** result
);

//Copy from origin
//
void mtapiRT_initNodeAttribute (
	MTAPI_OUT mtapi_node_attributes_t * attributes,
	MTAPI_OUT mtapi_status_t * status
	);

void mtapiRT_setNodeAttribute (
	MTAPI_INOUT mtapi_node_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_IN void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	);

void mtapiRT_getNodeAttribute (
	MTAPI_IN mtapi_node_t node,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_OUT void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	);

void mtapiRT_initActionAttribute (
	MTAPI_OUT mtapi_action_attributes_t * attributes,
	MTAPI_OUT mtapi_status_t * status
	);

void mtapiRT_setActionAttribute (
	MTAPI_INOUT mtapi_action_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_IN void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	);

void mtapiRT_getActionAttribute (
	MTAPI_IN mtapi_action_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_OUT void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	);
void mtapiRT_initQueueAttribute (
	MTAPI_OUT mtapi_queue_attributes_t * attributes,
	MTAPI_OUT mtapi_status_t * status
	);

void mtapiRT_setQueueAttribute (
	MTAPI_INOUT mtapi_queue_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_IN void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	);

void mtapiRT_getQueueAttribute (
	MTAPI_IN mtapi_queue_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_OUT void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	);
void mtapiRT_initTaskAttribute (
	MTAPI_OUT mtapi_task_attributes_t * attributes,
	MTAPI_OUT mtapi_status_t * status
	);

void mtapiRT_setTaskAttribute (
	MTAPI_INOUT mtapi_task_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_IN void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	);

void mtapiRT_getTaskAttribute (
	MTAPI_IN mtapi_task_attributes_t * attributes,
	MTAPI_IN mtapi_uint_t attribute_num,
	MTAPI_OUT void * attribute,
	MTAPI_IN mtapi_size_t attribute_size,
	MTAPI_OUT mtapi_status_t * status
	);

mtapi_boolean_t mtapiRT_nodeInitialized(mtapi_status_t * status);

void mtapiRT_initAffinity (
	MTAPI_OUT mtapi_affinity_t * mask,
	MTAPI_IN mtapi_boolean_t affinity,
	MTAPI_OUT mtapi_status_t * status
	);

void mtapiRT_setAffinity (
	MTAPI_INOUT mtapi_affinity_t * mask,
	MTAPI_IN mtapi_uint_t core_num,
	MTAPI_IN mtapi_boolean_t affinity,
	MTAPI_OUT mtapi_status_t * status
	);

mtapi_boolean_t mtapiRT_getAffinity (
	MTAPI_IN mtapi_affinity_t * mask,
	MTAPI_IN mtapi_uint_t core_num,
	MTAPI_OUT mtapi_status_t * status
	);
//End copy

int InsertTask(mtapiRT_TaskInfo_t *);

void ProcessTask(mtapiRT_TaskInfo_t *);

void SetTaskComplete(mtapiRT_TaskInfo_t *);
void * thread_func();
void * listener_func(void * args);

void * processer_func(void * args);
unsigned MarshallMessage(mtapiRT_Message_t *, char *);
mtapi_boolean_t UnmarshallMessage(char *, unsigned, mtapiRT_Message_t *);

mtapiRT_TaskInfo_t * GetTaskFromHead();

int InserMessage(mtapiRT_Message_t *);

int InsertMessageMaxPriority(mtapiRT_Message_t *);

mtapiRT_Message_t * GetMessageFromHead();

void ProcessMessage(mtapiRT_Message_t *, mcapi_endpoint_t *);

void StartRemoteTask(mtapiRT_Message_t *, mcapi_endpoint_t *);

void ReceiveRemoteTask(mtapiRT_Message_t *, mcapi_endpoint_t *);

void AwaitRemoteTask(mtapiRT_Message_t *, mcapi_endpoint_t *);

void AwaitLocalTask(mtapiRT_Message_t *, mcapi_endpoint_t *);

void CompleteRemoteTask(mtapiRT_Message_t *, mcapi_endpoint_t *);

void CompleteLocalTask(mtapiRT_Message_t *, mcapi_endpoint_t *);
void CompleteTaskWait(mtapiRT_Message_t *, mcapi_endpoint_t *);

void CreateAction(mtapiRT_Message_t *, mcapi_endpoint_t *);
void AddRemoteAction(mtapiRT_Message_t *, mcapi_endpoint_t *);
// global variables.

mtapi_node_t _nodeID;
mtapi_domain_t _domainID;


pthread_mutex_t taskListMutex;
pthread_mutex_t messageListMutex;
mtapiRT_JobInfo_t jobList[NUM_JOB];
mtapiRT_SetInfo_t nodeList[NUM_NODE];
mtapiRT_TaskInfo_t * taskListHead;
mtapiRT_TaskInfo_t * taskListTail;
mtapiRT_MessageEntry_t * messageListHead;
mtapiRT_MessageEntry_t * messageListTail;


#ifdef __cplusplus
}
#endif /* __cplusplus */

