// The pthread based skeleton program for the homework
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

#define OUTERREPS 1 
int nthreads, delaylength, innerreps;
double times[OUTERREPS+1];

#define OMP_MAX_NUM_THREADS 6 
#define OMP_LOG_MAX_NUM_THREADS 3
// centralized barrier and blocking barrier data structures
volatile int global_barrier_flag;
volatile int global_barrier_count;

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
double g_dblBarrierTime[OMP_MAX_NUM_THREADS];
//Global data for threads 0, 1, ..., 7
int g_ithreadid[OMP_MAX_NUM_THREADS];

typedef struct 
{
	volatile boolean *opponent;
	role_enum role;
	volatile boolean flag;
} round_t;

round_t rounds[OMP_MAX_NUM_THREADS][OMP_LOG_MAX_NUM_THREADS];

struct omp_v_thread
{
	volatile int parity; // any barrier thread private data
	volatile boolean sense; // any barrier thread private data
	volatile round_t *myrounds; // tournament barrier thread private data
}team[OMP_MAX_NUM_THREADS];

pthread_t p_threads[OMP_MAX_NUM_THREADS];
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
	if (firstcall) 
	{
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
	if (a < 0) 
	printf("%f \n",a);
}

void stats (double *mtp)
{
	double meantime, totaltime;
	int i;
	totaltime = 0.;
	for (i=1; i<=OUTERREPS; i++)
	{
	totaltime +=times[i];
	}
	meantime = totaltime / OUTERREPS;
	*mtp = meantime;
}

static inline int
ompc_atomic_inc(volatile int* value)
{
	return (__sync_fetch_and_add(value,1)+1);
}

/* end utilities */
void tour_barrier_init(int vpid, volatile round_t **myrounds)
{
	int k;
	(*myrounds) = &(rounds[vpid][0]);
	for(k = 0; k < OMP_LOG_MAX_NUM_THREADS; k++) 
		(*myrounds)[k].role = DROPOUT; /* default */
	for(k = 0; k < OMP_LOG_MAX_NUM_THREADS; k++) 
	{
		int vpid_mod_2_sup_k_plus_1 = vpid % (1 << (k+1));
		if (vpid_mod_2_sup_k_plus_1 == 0) 
		{
			int partner = vpid + (1 << k);
			if (partner < OMP_MAX_NUM_THREADS) 
				(*myrounds)[k].role = WINNER;
			else 
				(*myrounds)[k].role = NOOP;
		
			(*myrounds)[k].opponent = (boolean *) NULL;
		}
		else if (vpid_mod_2_sup_k_plus_1 == (1 << k)) 
		{
			(*myrounds)[k].role = LOSER;
			(*myrounds)[k].opponent = &(rounds[vpid - (1<<k)][k].flag);
			break;
		}
	}
	if (vpid == 0)
	{
		(*myrounds)[k-1].role = CHAMPION;
	}
	
	for(k = 0; k < OMP_LOG_MAX_NUM_THREADS; k++) 
	{
		(*myrounds)[k].flag = False;
	}
}

pthread_mutex_t barrier_lock;
pthread_cond_t barrier_cond;

void OMPC_BARRIER(int vpid)
{
	struct omp_v_thread *current_thd = &team[vpid];
	round_t* round=current_thd->myrounds;
	int i=0;
	for(;;) 
	{
		if(round->role & LOSER) 
		{
			*(round->opponent) = current_thd->sense;
			//printf("round = %d Loser 1, thread id = %d\n", i, vpid);
			while (champion_sense != current_thd->sense);
			//printf("round = %d Loser 2, thread id = %d\n", i, vpid);
				break;
		}
		else if (round->role & WINNER)
		{			
			//printf("round = %d WINNER 1, thread id = %d\n", i, vpid);
			while (round->flag != current_thd->sense);
			//printf("round = %d WINNER 2, thread id = %d\n", i, vpid);
		}
		else if (round->role & CHAMPION ) 
		{
			//printf("round = %d CHAMPION 1, thread id = %d\n", i, vpid);
			while (round->flag != current_thd->sense);
			//printf("round = %d CHAMPION 2, thread id = %d\n", i, vpid);
			champion_sense = current_thd->sense;
			break;
		}
		round++;
		i++;
	} // end for
	current_thd->sense ^= True;
}


void *print_message_function( void *ptr )
{
	int *i;
	int thread_id,n;
	int j,k=0;
	double start;
	double meantime;
	i = (int *)ptr;
	thread_id = *i;
	sleep(3);
	//printf("threadid %d\n", thread_id); //for debug
	for (k=0; k<=OUTERREPS; k++)
	{
		start = getclock();
		{
			for (j=0; j<innerreps; j++)
			{
				delay(delaylength);
				OMPC_BARRIER(thread_id);
			}
		}
		times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
	}
	stats (&meantime);
	//printf("TID:%d: BARRIER time = %f microseconds \n", thread_id,meantime);
	g_dblBarrierTime[thread_id] = meantime;
}

double max_array(double a[], int num_elements)
{
   int i;
   double max=0.0;
   for (i=0; i<num_elements; i++)
   {
	 if (a[i]>max)
	 {
	    max=a[i];
	 }
   }
   return(max);
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
	// tournament data initialization
	//printf("barrier init begun.\n"); //for debug
	for(i=0;i<OMP_MAX_NUM_THREADS;i++)
	{
		tour_barrier_init(i,&(team[i].myrounds));
		team[i].parity = 0;
		team[i].sense = True;
	}
	//printf("barrier init finished\n"); //for debug
	for(i=0;i<OMP_MAX_NUM_THREADS;i++)
	{
		g_ithreadid[i] = i;
		pthread_create( &p_threads[i], NULL, print_message_function, (void*) &g_ithreadid[i]);
		printf("create thread:%d\n", g_ithreadid[i]);
	}
	/* Wait till threads are complete before main continues. Unless we */
	/* wait we run the risk of executing an exit which will terminate */
	/* the process and all threads before the threads have completed. */
	for(i=0;i<OMP_MAX_NUM_THREADS;i++)
		pthread_join( p_threads[i], NULL);
	//find the max barrier time.
	printf("Max BARRIER time = %f microseconds \n", max_array(g_dblBarrierTime, OMP_MAX_NUM_THREADS));
	exit(0);
}
