#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "functions.h"


void slaveCreator(int amount, int* pidArray, int pipefdArray[][2], int parentfd[2], char* files[]);
static void parsePid(char * buff, const char *string);

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf("Input at least one file\n");
        return 0;
    }
    
    sleep(2);
    
    int filenum = argc - 1;
    int sharedBuffer = shm_open("Shared Buffer", O_CREAT | O_RDWR, 0);
    if(sharedBuffer == -1){
        perror("Error occured while trying to open the shared memory");
        exit(1);
    }

    if(ftruncate(sharedBuffer, filenum * 4096) == -1){
        perror("Error occured while trying excecuting ftruncate");
        exit(1);
    }

    void * mapped = mmap(NULL, 4096 * filenum, PROT_WRITE, MAP_SHARED, sharedBuffer, 0);

    if(mapped == (void*) -1){
        perror("Error occured while trying to map the shared memory block");
        exit(1);
    }

    sem_t *sem = sem_open("BufferSemaphore", O_CREAT, O_RDWR, 0);
    if(sem == SEM_FAILED) {
        perror("Error occured while trying to initialize a semaphore");
        exit(1);
    }
    
    int amount;
    // if (filenum < 8)
    //     amount = filenum;
    // else
    //     amount = 8;

    //amount = filenum;
    amount = 2;
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
    while (currentfile < filenum) {// while me quedan archivos
        //read(0, str, 4096);
        getFileChar(str, 4096);
        //printf("%s\n", str); <- Deberia ser write a resultado

        //Escribo a archivo
        fprintf(resultptr, str);

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
            int currentLen = strlen(argv[currentfile])+1;
            write(pipefdArray[index][1], argv[currentfile++], currentLen);// write nombre de siguiente archivo
        }
    }

    for (int i = 0; i < amount; i++) {
        write(pipefdArray[i][1], "-", sizeof("-")); // Aviso a todos mis hijos que terminen
    }

    for (int i = 0; i < amount; i++){
        waitpid(-1, NULL, 0); // Espero a que terminen amount de mis hijos
    }


    sem_close(sem);
    munmap(mapped, amount * 4096);
    shm_unlink("Shared Memory");
    close(sharedBuffer);
    fclose(resultptr);

    printf("PARENT FINISHED!\n");

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

        printf("Parent writing %s to pid: %d\n", files[j], pidArray[i]);
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

