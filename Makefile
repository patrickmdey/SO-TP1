all: Solver Slave

CC = gcc
CFLAGS = -g -Wall -std=gnu99 -pedantic-errors

Functions.o: functions.c
	$(CC) -c functions.c $(CFLAGS)

Solver: Solver.o functions.o
	$(CC) Solver.o functions.o -o Solver 

Solver.o: Solver.c
	$(CC) -c Solver.c $(CFLAGS)

Slave: Slave.o functions.o
	$(CC) Slave.o functions.o -o Slave

Slave.o: Slave.c
	$(CC) -c Slave.c $(CFLAGS)

clean:
	rm *.o Solver Slave Solver.o Slave.o functions.o