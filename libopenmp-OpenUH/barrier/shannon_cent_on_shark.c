#include <stdio.h>
#include <stdlib.h>
//#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#include<omp.h>

#define OUTERREPS 20
int nthreads, delaylength, innerreps;
double times[OUTERREPS+1] ;


#define OMP_MAX_NUM_THREADS 8
#define OMP_LOG_MAX_NUM_THREADS 3

// centralized barrier and blocking barrier data structures
volatile int global_barrier_flag=0;
volatile int global_barrier_count=0;

// tournament barrier data structures
#define False ((boolean) 0)
#define True ((boolean) 1)
typedef unsigned char role_enum;
#define LOSER ((role_enum) 1)
#define WINNER ((role_enum) 2)
#define CHAMPION ((role_enum) 4)
#define NOOP ((role_enum) 8)
#define DROPOUT ((role_enum) 16)
typedef unsigned char boolean;
volatile boolean champion_sense = False;

typedef struct {
	volatile boolean *opponent;
	role_enum role;
	volatile boolean flag;
}round_t;
round_t rounds[OMP_MAX_NUM_THREADS][OMP_LOG_MAX_NUM_THREADS];

struct omp_v_thread {
	volatile int parity; // any barrier thread private data
	volatile boolean sense; // any barrier thread private data
	volatile round_t *myrounds; // tournament barrier thread private data
}team[OMP_MAX_NUM_THREADS];


//pthread_t p_threads[OMP_MAX_NUM_THREADS];

/* utilities */
extern double get_time_of_day_(void);
extern void init_time_of_day_(void);
time_t starttime = 0;
double get_time_of_day_()
{
	struct timeval ts;
	double t;
	int err;
	err = gettimeofday(&ts, NULL);
	t = (double) (ts.tv_sec - starttime) + (double) ts.tv_usec * 1.0e-6;
	return t;
}
void init_time_of_day_()
{
	struct timeval ts;
	int err;
	err = gettimeofday(&ts, NULL);
	starttime = ts.tv_sec;
}

static int firstcall = 1;

double getclock()
{
	double time;
	double get_time_of_day_(void);
	void init_time_of_day_(void);
	if (firstcall) {
		init_time_of_day_();
		firstcall = 0;
	}
	time = get_time_of_day_();
	return time;
}

void delay(int delaylength)
{
	int i;
	float a=0.;
	for (i=0; i<delaylength; i++) 
		a+=i;
	if (a < 0) printf("%f \n",a);
}

void stats (double *mtp)
{
	double meantime, totaltime;
	int i;
	totaltime = 0.;
	for (i=1; i<=OUTERREPS; i++){
		totaltime +=times[i];
	}
	meantime = totaltime / OUTERREPS;
	*mtp = meantime;
}
/*
static inline int ompc_atomic_inc(volatile int* value)
{
	return (__sync_fetch_and_add(value,1)+1);
}
*/

/* end utilities */
/*
void tour_barrier_init(int vpid, volatile round_t **myrounds)
{
	int k;
	(*myrounds) = &(rounds[vpid][0]);
	for(k = 0; k < OMP_LOG_MAX_NUM_THREADS; k++) 
		(*myrounds)[k].role = DROPOUT;
*/
/*	for(k = 0; k < OMP_LOG_MAX_NUM_THREADS; k++) {
		int vpid_mod_2_sup_k_plus_1 = vpid % (1 << (k+1));
		if (vpid_mod_2_sup_k_plus_1 == 0) {
			int partner = vpid + (1 << k);
			if (partner < OMP_MAX_NUM_THREADS) 
				(*myrounds)[k].role = WINNER;
			else 
				(*myrounds)[k].role = NOOP;
			(*myrounds)[k].opponent = (boolean *) NULL;
		}
		else if (vpid_mod_2_sup_k_plus_1 == (1 << k)) {
			(*myrounds)[k].role = LOSER;
			(*myrounds)[k].opponent = &(rounds[vpid - (1<<k)][k].flag);
			break;
		}
	}

	if (vpid == 0) {
		(*myrounds)[k-1].role = CHAMPION;
	}

	for(k = 0; k < OMP_LOG_MAX_NUM_THREADS; k++) {
		(*myrounds)[k].flag = False;
	}
}
*/
//pthread_mutex_t barrier_lock;
//pthread_cond_t barrier_cond;

void OMPC_BARRIER();

//void *print_message_function( void *ptr )
void print_message_function()
 {
 #pragma omp parallel num_threads(OMP_MAX_NUM_THREADS)
 {
	//int *i;
	int thread_id,n;
	int j,k=0;
	double start;
	double meantime;
	//i = (int *)ptr;
	//thread_id = *i;
	thread_id=omp_get_thread_num();
	//sleep(3);
	for (k=0; k<=OUTERREPS; k++)
	{
		start = getclock();
		{
			for (j=0; j<innerreps; j++){
				delay(delaylength);
				OMPC_BARRIER();
			}
		}
		times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
	}
	stats (&meantime);
	printf("TID:%d: BARRIER time = %f microseconds \n", thread_id,meantime);	
 }
}

void OMPC_BARRIER()
{
	volatile int barrier_flag = global_barrier_flag;
	//volatile int new_count = ompc_atomic_inc(&global_barrier_count);
	#pragma omp atomic
		global_barrier_count++;
	
	volatile int new_count= global_barrier_count;
	if (new_count == OMP_MAX_NUM_THREADS){
	//	#pragma omp critical
		{
			global_barrier_count=0;
			global_barrier_flag = barrier_flag^1;
		}
	}
	else
	{
		while(barrier_flag==global_barrier_flag)
		{};
	}		
}



int main(void)
{
	int i;
	/* Create independent threads each of which will execute function */
	delaylength = 500;
	innerreps = 10000;
	// centralized barrier data initialization
	global_barrier_count=0;
	global_barrier_flag=0;
/*	
    // tournament data initialization
	for(i=0;i<OMP_MAX_NUM_THREADS;i++){
		tour_barrier_init(i,&(team[i].myrounds));
		team[i].parity = 0;
		team[i].sense = True;
	}
*/
	
//	for(i=0;i<OMP_MAX_NUM_THREADS;i++){
//		pthread_create( &p_threads[i], NULL, print_message_function, (void*) &i);
//	}
	/* Wait till threads are complete before main continues. Unless we */
	/* wait we run the risk of executing an exit which will terminate */
	/* the process and all threads before the threads have completed. */
/*	for(i=0;i<OMP_MAX_NUM_THREADS;i++)
		pthread_join( p_threads[i], NULL);
	exit(0);
	*/
	print_message_function();
}
