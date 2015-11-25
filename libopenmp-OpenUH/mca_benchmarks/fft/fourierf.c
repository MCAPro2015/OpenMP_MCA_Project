/*============================================================================

    fourierf.c  -  Don Cross <dcross@intersrv.com>

    http://www.intersrv.com/~dcross/fft.html

    Contains definitions for doing Fourier transforms
    and inverse Fourier transforms.

    This module performs operations on arrays of 'float'.

    Revision history:

1998 September 19 [Don Cross]
    Updated coding standards.
    Improved efficiency of trig calculations.

============================================================================*/

//#include <stdlib.h>
//#include <stdio.h>
#include <math.h>
#include <omp.h>
#include "fourier.h"
#include "ddcmath.h"

#define CHECKPOINTER(p)  CheckPointer(p,#p)

static void CheckPointer ( void *p, char *name )
{
    if ( p == NULL )
    {
        printf ("Error in fft_float():  %s == NULL\n", name );
        exit(1);
    }
}

    unsigned NumBits;    /* Number of bits needed to store indices */
    //unsigned i, j, k, n,m;
    int i, j, k, n,m;
    unsigned NumIter;
    unsigned sum=0;
    unsigned BlockSize,BlockEnd;
    double angle_numerator = 2.0 * DDC_PI;
    double tr, ti;     /* temp real, temp imaginary */
    double cm1, cm2, sm1, sm2,w; 
    double ar[3], ai[3];
    double temp;
    
    unsigned  NumSamples;
    int       InverseTransform;
    float    *RealIn;
    float    *ImagIn;
    float    *RealOut;
    float    *ImagOut;
  
void fft_float (
    unsigned  __NumSamples,
    int       __InverseTransform,
    float    *__RealIn,
    float    *__ImagIn,
    float    *__RealOut,
    float    *__ImagOut 
              )
    
{
    NumSamples=__NumSamples;
    InverseTransform = __InverseTransform;
    *RealIn = *__RealIn;
    *ImagIn = *__ImagIn;
    *RealOut = *__RealOut;
    *ImagOut = *__ImagOut; 
    if ( !IsPowerOfTwo(NumSamples) )
    {
        printf (
            "Error in fft():  NumSamples=%u is not power of two\n",
            NumSamples );

        exit(1);
    }

    if ( InverseTransform )
        angle_numerator = -angle_numerator;

    CHECKPOINTER ( RealIn );
    CHECKPOINTER ( RealOut );
    CHECKPOINTER ( ImagOut );

    NumBits = NumberOfBitsNeeded ( NumSamples );

    /*
    **   Do simultaneous data copy and bit-reversal ordering into outputs...
    */
    #pragma omp parallel private(i,j)
    {
      //printf("The number of threads %d\n", omp_get_num_threads());
      #pragma omp for schedule(static) nowait
      for ( i=0; i < NumSamples; i++ )
      {
        j = ReverseBits ( i, NumBits );
        RealOut[j] = RealIn[i];
        ImagOut[j] = (ImagIn == NULL) ? 0.0 : ImagIn[i];
      }
    }
    /*
    **   Do the FFT itself...
    */
    //BlockEnd = 1;
    NumIter = (unsigned)log2(NumSamples);
    //printf("NumIter=%d\n",NumIter); 
    
    //for ( BlockSize = 2; BlockSize <= NumSamples; BlockSize <<=1 )   //<<=1 to *2
    
    for(m=1; m<=NumIter;m++)
    {
        BlockSize = pow(2,m);
        BlockEnd = pow(2,m-1);
        //printf("BlockSize = %d, BlockEnd = %d\n",BlockSize,BlockEnd);
        double delta_angle = angle_numerator / (double)BlockSize;
        sm2 = sin ( -2 * delta_angle );
        sm1 = sin ( -delta_angle );
        cm2 = cos ( -2 * delta_angle );
        cm1 = cos ( -delta_angle );
        w = 2 * cm1;
        
        #pragma omp parallel for private(i,j,n,ar,ai) schedule(static)
        for ( i=0; i < NumSamples; i += BlockSize )
        {
            ar[2] = cm2;
            ar[1] = cm1;

            ai[2] = sm2;
            ai[1] = sm1;
            for ( j=i, n=0; n < BlockEnd; j++, n++ )
            {
                ar[0] = w*ar[1] - ar[2];
                ar[2] = ar[1];
                ar[1] = ar[0];

                ai[0] = w*ai[1] - ai[2];
                ai[2] = ai[1];
                ai[1] = ai[0];
                
                //printf("ar[0]=%d, ar[1]=%d, ar[2]=%d\n",ar[0],ar[1],ar[2]);
                k = j + BlockEnd;
                tr = ar[0]*RealOut[k] - ai[0]*ImagOut[k];
                ti = ar[0]*ImagOut[k] + ai[0]*RealOut[k];
                
                  RealOut[k] = RealOut[j] - tr;
                  ImagOut[k] = ImagOut[j] - ti;

                  RealOut[j] += tr;
                  ImagOut[j] += ti;
            }
        }

        //BlockEnd = BlockSize;
    }

    //printf("NumIter = %d, sum = %d\n",NumIter, sum);
    /*
    **   Need to normalize if inverse transform...
    */

    if ( InverseTransform )
    {
        double denom = (double)NumSamples;

        for ( i=0; i < NumSamples; i++ )
        {
            RealOut[i] /= denom;
            ImagOut[i] /= denom;
        }
    }
}


/*--- end of file fourierf.c ---*/
