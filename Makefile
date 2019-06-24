CFLAGS = -Wall -g -lm
CC = gcc
EXEC = 

compil: echeancier.o
	$(CC) jsq.c *.o -o jsq $(CFLAGS)
	$(CC) sita.c *.o -o sita $(CFLAGS)


run-simul: compil
	./jsq 

echeancier.o: echeancier.h
	$(CC) -c echeancier.c -o echeancier.o

clean:
	rm -f jsq echeancier.o

dir:
	mkdir res
	mkdir resf
	mkdir sres
	mkdir sresf