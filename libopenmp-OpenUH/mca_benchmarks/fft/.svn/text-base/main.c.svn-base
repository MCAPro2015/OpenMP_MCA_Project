#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <string.h>

void fft_float(unsigned, int, float*, float*, float*, float*);
//void fft_float(void);
    
    unsigned MAXSIZE;
	unsigned MAXWAVES;
	unsigned i,j;
	float *RealIn;
	float *ImagIn;
	float *RealOut;
	float *ImagOut;
	float *coeff;
	float *amp;
	int invfft=0;
    double sum_real=0.0;
    double sum_imag=0.0;
    struct timeval start, end;

int main(int argc, char *argv[]) {
    
	if (argc<3)
	{
		printf("Usage: fft <waves> <length> -i\n");
		printf("-i performs an inverse fft\n");
		printf("make <waves> random sinusoids");
		printf("<length> is the number of samples\n");
		exit(-1);
	}
	else if (argc==4)
		invfft = !strncmp(argv[3],"-i",2);
	MAXSIZE=atoi(argv[2]);
	MAXWAVES=atoi(argv[1]);
		
    srand(1);

    RealIn=(float*)malloc(sizeof(float)*MAXSIZE);
    ImagIn=(float*)malloc(sizeof(float)*MAXSIZE);
    RealOut=(float*)malloc(sizeof(float)*MAXSIZE);
    ImagOut=(float*)malloc(sizeof(float)*MAXSIZE);
    coeff=(float*)malloc(sizeof(float)*MAXWAVES);
    amp=(float*)malloc(sizeof(float)*MAXWAVES);

    /* Makes MAXWAVES waves of random amplitude and period */
	for(i=0;i<MAXWAVES;i++) 
	{
		coeff[i] = rand()%1000;
		amp[i] = rand()%1000;
	}
    
    for(i=0;i<MAXSIZE;i++) 
    {
    /*   RealIn[i]=rand();*/
	 RealIn[i]=0;
	 for(j=0;j<MAXWAVES;j++) 
	 {
		 /* randomly select sin or cos */
		 if (rand()%2)
		 {
		 		RealIn[i]+=coeff[j]*cos(amp[j]*i);
		 }
		 else
		 {
		 	RealIn[i]+=coeff[j]*sin(amp[j]*i);
		 }
  	      ImagIn[i]=0;
	 }
    }

    /*start timing*/
    gettimeofday(&start,NULL);
    double t1=start.tv_sec+(start.tv_usec/1000000.0);
    
    /* regular*/
    fft_float (MAXSIZE,invfft,RealIn,ImagIn,RealOut,ImagOut);
    //fft_float();
    
    /*end of timing*/
    gettimeofday(&end, NULL);
    double t2=end.tv_sec+(end.tv_usec/1000000.0);
    
    printf("The parallel wall time is %f sec\n", (t2-t1)); 
 
    printf("RealOut:\n");
    for (i=0;i<MAXSIZE;i++){
      //printf("%f \t", RealOut[i]);
      sum_real+=RealOut[i];
    }
    printf("sum_real=%f\n",sum_real);
    printf("\n");

    printf("ImagOut:\n");
    for (i=0;i<MAXSIZE;i++){
      //printf("%f \t", ImagOut[i]);
      sum_imag+=ImagOut[i];
    }
    printf("sum_imag = %f\n",sum_imag);
    printf("\n");

    free(RealIn);
    free(ImagIn);
    free(RealOut);
    free(ImagOut);
    free(coeff);
    free(amp);
    exit(0);
}
