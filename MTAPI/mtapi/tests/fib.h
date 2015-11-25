#include <stdlib.h>
#include <stdio.h>
#include "mcapi.h"
#include "mrapi.h"
#include "mtapi_runtime.h"
#include "mtapi.h"

enum {
	FUNCTION_0 = 0,
	FUNCTION_1,
	FUNCTION_2
};

const unsigned int NUM_NODES = 2;
const unsigned int NUM_DOMAINS = 3;
const unsigned int NUM_JOBS = 3;
const mtapi_job_id_t JOBS[] = {
	FUNCTION_0,
	FUNCTION_1,
	FUNCTION_2
};
mtapi_action_id_t ACTION_0[] = {FUNCTION_0, FUNCTION_1};
mtapi_action_id_t ACTION_1[] = {FUNCTION_1, FUNCTION_2};
mtapi_action_id_t ACTION_2[] = {FUNCTION_1, FUNCTION_2};

const mtapiRT_NodeInfo_t NODE_INFOS[] = {
	{0, 0, 0, 1, 2, ACTION_0},
	{1, 0, 0, 1, 2, ACTION_1},
//	{2, 0, 0, 1, 2, ACTION_2}
};



