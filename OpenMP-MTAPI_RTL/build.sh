gcc -g -O3 -shared -fpic -I/home/pengsun86/mtapi_build/include -L/home/pengsun86/mtapi_build/lib -Wl,-R/home/pengsun86/mtapi_build/lib -I. -lpthread -lrt -lembb_mtapi_c -lembb_base_c -o libgomp.so.1 team.c task.c parallel.c config.c
cp libgomp.so.1 /home/pengsun86/gcc_build/lib64/libgomp.so.1

