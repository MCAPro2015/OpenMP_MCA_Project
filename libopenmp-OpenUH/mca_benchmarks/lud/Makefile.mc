OBJ = lud.o lud_omp.w2c.o common.o
FILE = lud.c lud_omp.w2c common.c

CC = powerpc-linux-gnu-gcc
CFLAGS = -te500mc -O3 -std=c99
LDFLAGS = -lopenmp -lpthread -lm -lmrapi
C_INC = -I /home/pengsun86/opt/openuh-install/include/4.2
C_LIB = -L /home/pengsun86/mca-build/lib

lud: ${OBJ} Makefile
	$(CC) ${CFLAGS} $(C_INC) ${OBJ} ${LDFLAGS} $(C_LIB) -o lud_mca
lud.o: lud.c
	$(CC) ${CFLAGS} $(C_INC) -c lud.c
lud_omp.w2c.o: lud_omp.w2c.c
	$(CC) ${CFLAGS} $(C_INC) -c lud_omp.w2c.c
common.o: common.c
	$(CC) ${CFLAGS} $(C_INC) -c common.c


clean:
	rm -rf *.o fft output* #*.w2c.c *.w2c.h
