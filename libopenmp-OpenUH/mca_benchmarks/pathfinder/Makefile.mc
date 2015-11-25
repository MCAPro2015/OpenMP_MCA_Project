OBJ = pathfinder.o pathfinder_kernel.w2c.o 
FILE = pathfinder.c pathfinder_kernel.w2c.c 

CC = powerpc-linux-gnu-gcc
CFLAGS = -te500mc -O3 -std=c99
LDFLAGS = -lopenmp -lpthread -lm -lmrapi
C_INC = -I /home/pengsun86/opt/openuh-install/include/4.2
C_LIB = -L /home/pengsun86/mca-build/lib

pathfinder: ${OBJ} Makefile
	$(CC) ${CFLAGS} $(C_INC) ${OBJ} ${LDFLAGS} $(C_LIB) -o pathfinder_mca
pathfinder.o: pathfinder.c
	$(CC) ${CFLAGS} $(C_INC) -c pathfinder.c
pathfinder_kernel.w2c.o: pathfinder_kernel.w2c.c
	$(CC) ${CFLAGS} $(C_INC) -c pathfinder_kernel.w2c.c

clean:
	rm -rf *.o pathfinder_mca output* #*.w2c.c *.w2c.h
