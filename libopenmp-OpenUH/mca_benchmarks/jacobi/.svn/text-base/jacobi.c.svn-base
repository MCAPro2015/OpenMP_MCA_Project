#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

int main (int argc, char *argv[]);
void jacobi (int n, double a[], double b[], double x[], double x_old[]);
void r8vec_uniform_01 (int n, int *seed, double r[]);

/******************************************************************************/

int main (int argc, char *argv[])

/******************************************************************************/
/*
  Purpose:

    MAIN is the main program for JACOBI.
*/
{
  double *a;
  double *b;
  int i;
  int j;
  int n;
  double row_sum;
  int seed;
  double *x;
  double *x_old;
    
  if (argc != 2) {
	fprintf(stderr, "Usage: jacobi <n>\n");
    exit(1);
  }
	
  n = atoi(argv[1]);

  a = (double*)malloc(n*n*sizeof(double));
  b = (double*)malloc(n*sizeof(double));
  x = (double*)malloc(n*sizeof(double));
  x_old = (double*)malloc(n*sizeof(double));

  printf("\n");
  printf("JACOBI\n");
  printf("  C version\n");
  printf("  Problem size N = %d\n", n);

  seed = 123456789;

  r8vec_uniform_01(n*n, &seed, a);

  for(i = 0; i < n; i++)
  {
    row_sum = 0.0;
    for(j = 0; j < n; j++)
    {
      row_sum = row_sum + fabs(a[i+j*n]);
    }
    a[i+i*n] = 1.0 + 2.0 * row_sum;
  }

  for(i = 0; i < n; i++)
  {
    x[i] = (double)(i + 1);
  }

  for(i = 0; i < n; i++)
  {
    b[i] = 0.0;
    for(j = 0; j < n; j++)
    {
      b[i] = b[i] + a[i+j*n] * x[j];
    }
  }

  r8vec_uniform_01(n, &seed, x);

  jacobi (n, a, b, x, x_old);

  free(a);
  free(b);
  free(x);
  free(x_old);

  return 0;
}



extern double kernel_extern();



/******************************************************************************/

void jacobi(int n, double a[], double b[], double x[], double x_old[])

/******************************************************************************/
/*
  Purpose:

    JACOBI carries out the Jacobi iteration.
*/

{
  double axi;
  double diff;
  double diff_tol;
  int i;
  int it;
  int it_max = 100;
  int j;
  double r8_epsilon = 1.0E-14;
  struct timeval tim;
  double start;
  double end;

  diff_tol = 0.0;
  for(i = 0; i < n; i++)
  {
    diff_tol = diff_tol + fabs(b[i]);
  }
  diff_tol = (diff_tol + 1.0) * r8_epsilon;

  gettimeofday(&tim, NULL);
  start = tim.tv_sec + (tim.tv_usec/1000000.0);

  diff = kernel_extern(x_old,x,b,a,it_max,n,diff
          ,diff_tol);

  gettimeofday(&tim, NULL);
  end = tim.tv_sec + (tim.tv_usec/1000000.0);

  printf("\n");
  printf("DIFF     = %e\n", diff);
  printf("DIFF_TOL = %e\n", diff_tol);
  printf("Time     = %.2lf s\n", end-start);

  printf("\n");
  printf("  First 10 elements of estimated solution:\n");
  printf("\n");

  for(i = 0; i < 10; i++)
  {
    printf("  %8d  %14f\n", i, x[i]);
  }

  return;
}
/******************************************************************************/

void r8vec_uniform_01 (int n, int *seed, double r[])

/******************************************************************************/
/*
  Purpose:

    R8VEC_UNIFORM_01 returns a unit pseudorandom vector.

  Parameters:

    Input, int N, the number of entries in the vector.

    Input/output, int *SEED, a seed for the random number generator.

    Output, double R[N], the vector of pseudorandom values.
*/
{
  int i;
  int i4_huge = 2147483647;
  int k;

  if (*seed == 0)
  {
    printf("\n");
    printf("R8VEC_UNIFORM_01 - Fatal error!\n");
    printf("  Input value of SEED = 0.\n");
    exit(1);
  }

  for(i = 0; i < n; i++)
  {
    k = *seed / 127773;

    *seed = 16807*(*seed - k * 127773) - k * 2836;

    if(*seed < 0)
    {
      *seed = *seed + i4_huge;
    }

    r[i] = (double)(*seed) * 4.656612875E-10;
  }

  return;
}
