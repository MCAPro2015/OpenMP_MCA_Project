#include<omp.h> 
#define MIN(a, b) ((a)<=(b) ? (a) : (b))
int min;
int n;
int t_g;
int cols_g;
int *src_g;
int *dst_g;
extern int **wall;
void kernel(int t,int cols, int *src, int *dst){
    src_g=src;
    dst_g=dst;
    cols_g = cols;
    t_g = t;
#pragma omp parallel for private(min,n)
        for(n = 0; n < cols_g; n++){
          min = src_g[n];
          if (n > 0)
            min = MIN(min, src_g[n-1]);
          if (n < cols_g-1)
            min = MIN(min, src_g[n+1]);
          dst_g[n] = wall[t_g+1][n]+min;
        }
}
