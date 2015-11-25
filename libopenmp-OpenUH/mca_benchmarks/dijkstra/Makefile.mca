OBJ = Dijkstra.o Dijkstra_kernel.w2c.o
FILE = Dijkstra.c Dijkstra_kernel.w2c.c

CC = powerpc-linux-gnu-gcc
CFLAGS = -te500mc -O3 -std=c99 
LDFLAGS = -lopenmp -lpthread -lm -lmrapi
C_INC = -I /home/pengsun86/opt/openuh-install/include/4.2
C_LIB = -L /home/pengsun86/mca-build/lib -L /home/pengsun86/opt/openuh-install/lib/gcc-lib/x86_64-open64-linux/4.2

Dijkstra: ${OBJ} Makefile
	$(CC) ${CFLAGS} ${OBJ} $(C_INC) $(LDFLAGS) $(C_LIB) -o Dijkstra_mca

Dijkstra.o: Dijkstra.c
	$(CC) ${CFLAGS} $(C_INC) -c Dijkstra.c
Dijkstra_kernel.w2c.o: Dijkstra_kernel.w2c.c
	$(CC) ${CFLAGS} $(C_INC) -c Dijkstra_kernel.w2c.c

clean:
	rm -rf *.o fft output* #*.w2c.c *.w2c.h
