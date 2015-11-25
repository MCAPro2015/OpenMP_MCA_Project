OBJ = jacobi.o jacobi_kernel.w2c.o
FILE = jacobi.c jacobi_kernel.w2c.c

CC = gcc
CFLAGS = -std=c99 -O3 
LDFLAGS = -lpthread -lm -lopenmp
C_INC = -I /home/pengsun86/opt/openuh-install/include/4.2
C_LIB = -L /home/pengsun86/opt/openuh-install/lib/gcc-lib/x86_64-open64-linux/4.2

jacobi: ${OBJ} Makefile
	$(CC) ${CFLAGS} ${OBJ} $(LDFLAGS) $(C_LIB) -o jacobi

jacobi.o: jacobi.c
	$(CC) ${CFLAGS} $(C_INC) -c jacobi.c
jacobi_kernel.w2c.o: jacobi_kernel.w2c.c
	$(CC) ${CFLAGS} $(C_INC) -c jacobi_kernel.w2c.c

clean:
	rm -rf *.o fft output* #*.w2c.c *.w2c.h
