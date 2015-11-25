OBJ = main.o fftmisc.o  fourierf.w2c.o
#FILE = main.c fftmisc.c  fourierf.w2c.c

#GCC Compiler
#CC = powerpc-linux-gnu-gcc
#CFLAGS = -std=c99 -O3 -te500v2
#LDFLAGS = -lm -fopenmp

#Cross-compile
CC = powerpc-linux-gnu-gcc
CFLAGS= -O3 -te500mc
LDFLAGS= -lpthread -lm -lmrapi -lopenmp 
C_INC  = -I /home/pengsun86/opt/openuh-install/include/4.2 
C_LIB = -L /home/pengsun86/opt/openuh-install/lib/gcc-lib/x86_64-open64-linux/4.2 -L /home/pengsun86/mca-build/lib

fft: ${OBJ} Makefile
	$(CC) ${CFLAGS} ${OBJ} $(LDFLAGS) $(C_LIB) -o fft_mca
fftmisc.o: 
	$(CC) ${CFLAGS} $(C_INC) -c fftmisc.c
fourierf.w2c.o: 
	$(CC) ${CFLAGS} $(C_INC) -o fourierf.w2c.o -c fourierf.w2c.c
main.o:
	$(CC) ${CFLAGS} $(C_INC) -c main.c

clean:
	rm -rf *.o fft_mca output*# *.w2c.c *.w2c.h
