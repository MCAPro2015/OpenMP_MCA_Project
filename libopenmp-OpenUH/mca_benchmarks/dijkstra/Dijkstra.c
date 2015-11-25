// Dijkstra.c

// OpenMP example program:  Dijkstra shortest-path finder in a
// bidirectional graph; finds the shortest path from vertex 0 to all
// others

// usage:  dijkstra nv print

// where nv is the size of the graph, and print is 1 if graph and min
// distances are to be printed out, 0 otherwise

#include <omp.h>
#include <stdlib.h>
#include <sys/time.h>
struct timeval start,end;

// global variables, shared by all threads by default

int nv,  // number of vertices
    *notdone, // vertices not checked yet
    nth,  // number of threads
    chunk,  // number of vertices handled by each thread
    md,  // current min over all threads
    mv,  // vertex which achieves that min
    largeint = -1;  // max possible unsigned int

unsigned *ohd,  // 1-hop distances between vertices; "ohd[i][j]" is
         // ohd[i*nv+j]
         *mind;  // min distances found so far

void init(int ac, char **av)
{  int i,j,tmp;
   nv = atoi(av[1]);
   ohd = malloc(nv*nv*sizeof(int));
   mind = malloc(nv*sizeof(int));
   notdone = malloc(nv*sizeof(int)); 
   // random graph
   for (i = 0; i < nv; i++)  
      for (j = i; j < nv; j++)  {
         if (j == i) ohd[i*nv+i] = 0;
         else  {
            ohd[nv*i+j] = rand() % 20;
            ohd[nv*j+i] = ohd[nv*i+j];
         }
      }
   for (i = 1; i < nv; i++)  {
      notdone[i] = 1;
      mind[i] = ohd[i];
   }
}

// finds closest to 0 among notdone, among s through e
void findmymin(int s, int e, unsigned *d, int *v)
{  int i;
   *d = largeint; 
   for (i = s; i <= e; i++)
      if (notdone[i] && mind[i] < *d)  {
         *d = ohd[i];
         *v = i;
      }
}

// for each i in [s,e], ask whether a shorter path to i exists, through
// mv
void updatemind(int s, int e)
{  int i;
   for (i = s; i <= e; i++)
      if (mind[mv] + ohd[mv*nv+i] < mind[i])  
         mind[i] = mind[mv] + ohd[mv*nv+i];
}
extern void dowork();

int main(int argc, char **argv)
{  int i,j,print;
   init(argc,argv);
 /*start timing*/
   gettimeofday(&start,0);
   double t1=start.tv_sec + (start.tv_usec/1000000.0);
   // parallel
   dowork();  
   // back to single thread
 /*end of timing*/
   gettimeofday(&end,0);
   double t2=end.tv_sec+(end.tv_usec/1000000.0);

   printf("The parallel wall time is %f sec\n",(t2-t1));
if(argc>2){
   print = atoi(argv[2]);
   if (print)  {
      printf("graph weights:\n");
      for (i = 0; i < nv; i++)  {
         for (j = 0; j < nv; j++)  
            printf("%u  ",ohd[nv*i+j]);
         printf("\n");
      }
      printf("minimum distances:\n");
      for (i = 1; i < nv; i++)
         printf("%u\n",mind[i]);
   }
}
}
