#include <omp.h>
extern int nv;  // number of vertices
 extern int   *notdone; // vertices not checked yet
 extern int   nth;  // number of threads
  extern int  chunk;  // number of vertices handled by each thread
 extern int   md;  // current min over all threads
 extern int   mv;  // vertex which achieves that min
 extern int   largeint;  // max possible unsigned int

extern unsigned *ohd;  // 1-hop distances between vertices; "ohd[i][j]" is ohd[i*nv+j]
extern unsigned *mind;  // min distances found so far

extern void init();
extern void findmymin();
extern void updatemind();

 int startv,endv,  // start, end vertices for my thread
          step,  // whole procedure goes nv steps
          mymv,  // vertex which attains the min value in my chunk
          me;
 unsigned int mymd;  // min value found by this thread

void dowork()
{ 
#pragma threadprivate(startv,endv,step, mymv,me,mymd)
   #pragma omp parallel 
   {
       me = omp_get_thread_num();
    #pragma omp master   
      {  nth = omp_get_num_threads();  
         if (nv % nth != 0) {
            printf("nv must be divisible by nth\n");
            exit(1);
         }
         chunk = nv/nth;
         //printf("nv = %d, chunk = %d\n", nv, chunk);  
         //printf("there are %d threads\n",nth);  
      }
      startv = me * chunk; 
      endv = startv + chunk - 1;
      for (step = 0; step < nv; step++){
         // find closest vertex to 0 among notdone; each thread finds
         // closest in its group, then we find overall closest
         #pragma omp master 
         {  md = largeint; mv = 0;  }
         findmymin(startv,endv,&mymd,&mymv);
         // update overall min if mine is smaller
         #pragma omp critical  
         {  if (mymd < md)  
               {  md = mymd; mv = mymv;  }
         }
         #pragma omp barrier 
         // mark new vertex as done 
         #pragma omp master 
         {  notdone[mv] = 0;  }
         // now update my section of mind
         updatemind(startv,endv);
         #pragma omp barrier 
         }
   }
}



