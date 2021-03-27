all: Solver Slave

CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic-errors

Solver: Solver.o
	$(CC) Solver.o -o Solver 

Solver.o: Solver.c
	$(CC) -c Solver.c $(CFLAGS)

Slave: Slave.o
	$(CC) Slave.o -o Slave

Slave.o: Slave.c
	$(CC) -c Slave.c $(CFLAGS)

clean:
	rm *.o Solver Slave