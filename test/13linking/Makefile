# complitime
intc: mymalloc.c malloc.h int.c
	gcc -Wall -DCOMPILETIME -c mymalloc.c
	gcc -Wall -I. -o intc int.c mymalloc.o

# compiletime
runc: intc
	./intc 10 100 1000

# linktime
intl: mymalloc2.c malloc.h int.c
	gcc -Wall -DLINKTIME -c mymalloc2.c
	gcc -Wall -c int.c
	gcc -Wall -Wl,--wrap,malloc -Wl,--wrap,free -o intl int.o mymalloc2.o

# linktime
runl: intl
	./intl 10 100 1000

# loadtime/runtime
intr: mymalloc3.c malloc.h int.c
	gcc -Wall -DRUNTIME -shared -fpic -o mymalloc3.so mymalloc3.c -ldl
	gcc -Wall -o intr int.c

# loadtime/runtime
runr: intr
	(LD_PRELOAD="./mymalloc3.so" ./intr 10 100 1000)

clean:
	rm -rf *.o
	rm -rf *.so
	rm -f intc
	rm -f intl
	rm -f intr

