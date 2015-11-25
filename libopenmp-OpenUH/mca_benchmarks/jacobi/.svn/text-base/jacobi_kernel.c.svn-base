#include<omp.h>
int it;
int i;
int j;
//extern int it_max;
//extern int n;
double axi;
//double diff;
//extern double diff_tol;
extern double fabs();
double *g_x_old,*g_x,*g_b,*g_a;
int g_it_max,g_n;
double g_diff,g_diff_tol;



double kernel_extern(double x_old[],double x[], double b[],
        double a[],int it_max,int n,double diff,double diff_tol)
{
    g_x_old = x_old;
    g_x = x;
    g_b = b;
    g_a = a;
    g_it_max = it_max;
    g_n = n;
    g_diff = diff;
    g_diff_tol = diff_tol;
    
  for(it = 1; it <= g_it_max; it++)
  {
#pragma omp parallel private(i,axi,j)
{
#pragma omp for
    for(i = 0; i < g_n; i++)
    {
      g_x_old[i] = g_x[i];
    }
#pragma omp barrier

#pragma omp for
    for(i = 0; i < g_n; i++)
    {
      axi = 0.0;
      for (j = 0; j < g_n; j++)
      {
        axi = axi + g_a[i+j*g_n] * g_x_old[j];
      }
      g_x[i] = g_x_old[i] + (g_b[i] - axi) / g_a[i+i*g_n];
    }
 }
    g_diff = 0.0;
    for(i = 0; i < g_n; i++)
    {
      g_diff = g_diff + fabs(g_x[i] - g_x_old[i]);
    }

//    printf( "  %8d  %14e\n", it, diff);

    if(g_diff <= g_diff_tol)
    {
      break;
    }
  }
   return g_diff;
} 
