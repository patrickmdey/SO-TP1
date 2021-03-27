all: Solver Slave Vista

CC = gcc
CFLAGS = -g -std=gnu99 -Wall -pedantic-errors
OFLAGS = -lpthread -lrt

Functions.o: functions.c
	$(CC) -c functions.c $(CFLAGS)

Solver: Solver.o functions.o
	$(CC) Solver.o functions.o -o Solver $(OFLAGS)

Solver.o: Solver.c
	$(CC) -c Solver.c $(CFLAGS)

Slave: Slave.o functions.o
	$(CC) Slave.o functions.o -o Slave

Slave.o: Slave.c
	$(CC) -c Slave.c $(CFLAGS)

Vista: Vista.o
	$(CC) Vista.o -o Vista $(OFLAGS)

Vista.o: Vista.c
	$(CC) -c Vista.c $(CFLAGS)

clean:
	rm *.o Solver Slave Vista