CFLAGS = -Wall -g -lm
CC = gcc
EXEC = 

compil: echeancier.o
	$(CC) jsq.c *.o -o jsq $(CFLAGS)


run-simul: compil
	./jsq 

echeancier.o: echeancier.h
	$(CC) -c echeancier.c -o echeancier.o

clean:
	rm -f jsq echeancier.o
