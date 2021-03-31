all: bin/Solver bin/Slave bin/Vista

CC = gcc
CFLAGS = -g -std=c99 -Wall -pedantic-errors
OFLAGS = -lpthread -lrt

bin/Functions.o: src/functions.c
	$(CC) -c src/functions.c -o bin/functions.o $(CFLAGS)

bin/Solver: bin/Solver.o bin/Functions.o
	$(CC) bin/Solver.o bin/functions.o -o bin/Solver $(OFLAGS)

bin/Solver.o: src/Solver.c
	$(CC) -c src/Solver.c -o bin/Solver.o $(CFLAGS) -I./include

bin/Slave: bin/Slave.o bin/Functions.o
	$(CC) bin/Slave.o bin/functions.o -o bin/Slave

bin/Slave.o: src/Slave.c
	$(CC) -c src/Slave.c -o bin/Slave.o $(CFLAGS) -I./include

bin/Vista: bin/Vista.o
	$(CC) bin/Vista.o -o bin/Vista $(OFLAGS)

bin/Vista.o: src/Vista.c
	$(CC) -c src/Vista.c -o bin/Vista.o $(CFLAGS) -I./include

clean:
	rm bin/*