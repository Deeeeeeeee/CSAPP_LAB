shellex: csapp.c csapp.h shellex.c sigint.c procmask1.c procmask2.c waitforsignal.c
	gcc csapp.c shellex.c -o shellex -pthread
	gcc csapp.c sigint.c -o sigint -pthread
	gcc csapp.c forks.c -o forks -pthread
	gcc csapp.c procmask1.c -o procmask1 -pthread
	gcc csapp.c procmask2.c -o procmask2 -pthread
	gcc csapp.c waitforsignal.c -o waitforsignal -pthread

run: shellex
	./shellex

clean:
	rm -rf *.o
	rm -f shellex
	rm -f sigint
	rm -f forks
	rm -f procmask1
	rm -f procmask2
	rm -f waitforsignal

