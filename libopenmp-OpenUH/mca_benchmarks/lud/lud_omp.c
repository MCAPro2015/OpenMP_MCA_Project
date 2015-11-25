//#include <stdio.h>
#include <omp.h>

//extern int omp_num_threads;
     int i,j,k;
     float sum;
float *a_g;
int size_g;

void lud_omp(float *a, int size)
{
    a_g=a;
    size_g = size;
//	 printf("num of threads = %d\n", omp_num_threads);
     for (i=0; i <size_g; i++){
//		omp_set_num_threads(omp_num_threads);
#pragma omp parallel for default(none) \
         private(j,k,sum) shared(size_g,i,a_g) 
         for (j=i; j <size_g; j++){
             sum=a_g[i*size_g+j];
             for (k=0; k<i; k++) sum -= a_g[i*size_g+k]*a_g[k*size_g+j];
             a_g[i*size_g+j]=sum;
         }
#pragma omp parallel for default(none) \
         private(j,k,sum) shared(size_g,i,a_g) 
         for (j=i+1;j<size_g; j++){
             sum=a_g[j*size_g+i];
             for (k=0; k<i; k++) sum -=a_g[j*size_g+k]*a_g[k*size_g+i];
             a_g[j*size_g+i]=sum/a_g[i*size_g+i];
         }
     }
}
