/*
Naive matrix-matrix multiplication(mmm)
*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
//#include <omp.h>


int i,j,k;
double **a, **b, **c;
void init();
void mmm();
void verify();
int threads;

//void result(int); 
int N;
int main(int argc, char *argv[])
{
    //printf("the number of arguments is: %d \n",argc);
	if(argc != 2)
	{
		//fprintf(stderr, "usage: %s <#dimensions> \n", argv[0]);
		//exit(EXIT_FAILURE);
		printf("usage: %s <#dimensions> \n", argv[0]);
        exit(-1);
	}
	N=atoi(argv[1]);
	a = malloc(N * sizeof(double *));
	b = malloc(N * sizeof(double *));
	c = malloc(N * sizeof(double *));
	
    for(i = 0; i < N; i++)
	{
		a[i] = malloc(N * sizeof(double));
		b[i] = malloc(N * sizeof(double));
		c[i] = malloc(N * sizeof(double));
	}

    struct timeval start, end;

	printf ("The dimension value is %d: \n",atoi(argv[1]));
    #pragma omp parallel
    {
      #pragma omp master
      {
        //threads = omp_get_num_threads();
        //printf("num of threads is %d\n",threads);
      }
    }    
    init();
    
    gettimeofday(&start,NULL);
    double t1=start.tv_sec+(start.tv_usec/1000000.0);
	mmm();
    
    gettimeofday(&end, NULL);
    double t2=end.tv_sec+(end.tv_usec/1000000.0);
	
    verify();
    
    printf("the parallel wall time is %lf sec\n", t2-t1);
	return 0;
}

void init()
{
    //#pragma omp parallel private(i,j,k)
    //{
      //#pragma omp for schedule(static)
        for (i=0;i<N;i++)
		  for(j=0;j<N;j++){
			a[i][j]=i;
            b[i][j]=i;
            c[i][j]=0.0;
          }
/*      
      //#pragma omp for schedule(static)
	    for (i=0;i<N;i++)
		  for(j=0;j<N;j++)
			b[i][j]=1;
      //#pragma omp for schedule (static)
	    for (i=0;i<N;i++)
		  for(j=0;j<N;j++)
			c[i][j]=0.0;
    }
    */
}
void mmm()
{
    #pragma omp parallel private (i,j,k) num_threads(threads)
    { 
      #pragma omp for schedule(static) 
        for (i = 0; i < N; i++)
		    for (k = 0; k < N; k++) 
	          for (j = 0; j < N; j++){
				c[i][j]= c[i][j]+a[i][k]*b[k][j];
                //printf("c[%d][%d]=%f\n",i,j,c[i][j]);
              }
    }
}

double sum = 0.0;

void verify()
{
    //#pragma omp parallel private(i,j) num_threads(2)//for schedule(static) reduction(+:sum)
    {
      //#pragma omp for schedule(static) reduction(+:sum)
        for (i=0;i<N;i++){
		  for(j=0;j<N;j++){
			  sum+=c[i][j];
          }
        }
    
    }
	printf("sum of c[i][j] is %f\n",sum);
}
