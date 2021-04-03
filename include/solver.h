#ifndef SOLVER_H
#define SOLVER_H

#define MAX_SLAVES 8
#define PID_MAX_LEN 8

void slaveCreator(int amount, int* pidArray, int pipefdArray[][2], int parentfd[2], char* files[]);
static void parsePid(char * buff, const char *string);
static char * openshm(int shm_size, int * shm_fd);

#endif