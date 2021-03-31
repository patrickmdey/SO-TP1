// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define _BSD_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>

#include "functions.h"
#include "shared_memory.h"

void slaveCreator(int amount, int* pidArray, int pipefdArray[][2], int parentfd[2], char* files[]);
static void parsePid(char * buff, const char *string);

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf("Input at least one file\n");
        return 0;
    }
    
    int filenum = argc - 1;
    int shm_size = 4096 * filenum;
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0);
    if(shm_fd == -1){
        perror("Error occured while trying to open the shared memory");
        exit(1);
    }

    sem_t * sem = sem_open(SEM_NAME, O_CREAT | O_RDWR, S_IRGRP | S_IWGRP, 0);
    if(sem == SEM_FAILED) {
        perror("Error occured while trying to initialize a semaphore");
        exit(1);
    }

    if(ftruncate(shm_fd, shm_size) == -1){
        perror("Error occured while trying excecuting ftruncate");
        exit(1);
    }

    char * shmem = (char *) mmap(NULL, shm_size, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if(shmem == (void*) -1){
        perror("Error occured while trying to map the shared memory block");
        exit(1);
    }

    sleep(2);

    write(1, &shm_size, sizeof(int));
    //write(1, '\n, 2);

    

    //TODO: Ver como hacemos lo de la cantidad de procesos
    int amount;
    // if (filenum < 8)
    //     amount = filenum;
    // else
    //     amount = 8;

    //amount = filenum;
    amount = 3;
    int currentfile = amount + 1;

    pid_t pidArray[amount];
    int pipefdArray[amount][2];
    int parentfd[2];
    if (pipe(parentfd) == -1) {
        perror("Error occured while excecuting pipe");
        exit(1);
    }

    int aux = parentfd[0];
    if (dup2(parentfd[0], 0) == -1) {
        perror("Error duping pipe");
        exit(1);
    }

    slaveCreator(amount, pidArray, pipefdArray, parentfd, argv);
    close(aux);
    close(parentfd[1]);
    //Mientras queden archivos

    FILE * resultptr = fopen("./result.txt", "w");
    if(resultptr == NULL){
        perror("Error occured while trying to open result.txt");
        exit(1);
    }

    char str[4096] = { 0 };
    for (int readFiles = 0; readFiles < filenum; readFiles++) {// while me quedan archivos por leer
        //read(0, str, 4096);
        getFileChar(str, 4096);
        //printf("%s\n", str); <- Deberia ser write a resultado

        //Escribo a archivo
        int size = fprintf(resultptr, "%s", str);

        //Escribo a vista
        memcpy(shmem, str, size + 1);
        shmem += size + 1;
        sem_post(sem);

        //Semaforo y escribo a shmem
        /*sem_wait(sem);
        dprintf(sharedBuffer, "%s", str);
        sem_post(sem);*/

        char pidStr[10] = {0};
        parsePid(pidStr, str);
        pid_t pid = atoi(pidStr);
        //printf("Parent recieves from child [pid: %d]\n", pid);
        int found = 0;
        int index;
        for (int j = 0; j < amount && found != 1; j++) {
            if (pidArray[j] == pid) {
                found = 1;
                index = j;
            }
        }
        if (found) {
            //printf("Parent writting file: %s to pid: %d\n", argv[currentfile], pid);
            if (currentfile <= filenum) {
                int currentLen = strlen(argv[currentfile])+1;
                write(pipefdArray[index][1], argv[currentfile++], currentLen);// write nombre de siguiente archivo
            } else {
                close(pipefdArray[index][1]);
            }
        }
    }

    /*for (int i = 0; i < amount; i++) {
        write(pipefdArray[i][1], "-", sizeof("-")); // Aviso a todos mis hijos que terminen
    }*/

    for (int i = 0; i < amount; i++){
        waitpid(-1, NULL, 0); // Espero a que terminen amount de mis hijos
    }


    shm_unlink(SHM_NAME);
    close(shm_fd); // Mando EOF
    sem_post(sem);

    sem_close(sem);
    sem_unlink(SEM_NAME);
    fclose(resultptr);

    //printf("PARENT FINISHED!\n");

    return 0;
}

void slaveCreator(int amount, pid_t* pidArray, int pipefdArray[][2], int parentfd[2], char* files[]) {
    int i, j = 1;

    for (i = 0; i < amount; i++, j++) {

        if (pipe(pipefdArray[i]) == -1) {
            perror("Error occured while excecuting pipe");
            exit(1);
        }

        pidArray[i] = fork();

        if (pidArray[i] == -1) {
            perror("Error occured while excecuting fork\n");
            exit(1);
        }

        if (pidArray[i] == 0) {
            close(pipefdArray[i][1]);
            close(parentfd[0]);
            int aux1 = pipefdArray[i][0];
            int aux2 = parentfd[1];
            if (dup2(pipefdArray[i][0], 0) == -1 || dup2(parentfd[1], 1) == -1) {
                perror("Error duping pipe");
                exit(1);
            }
            close(aux1);
            close(aux2);

            char* args[] = { "./Slave", NULL };
            if (execv(args[0], args) == -1) {
                perror("Error occured while executing slave process\n");
                exit(1);
            }
        }

        // printf("Parent writing %s to pid: %d\n", files[j], pidArray[i]);
        write(pipefdArray[i][1], files[j], strlen(files[j]) + 1);
    }

}


static void parsePid(char * buff, const char *string){
    int j = 0;
    for(int i = 5; string[i] != '\n'; i++){
        buff[j++] = string[i];
    }
    buff[j] = 0;
}
