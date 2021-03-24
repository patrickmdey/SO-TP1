#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

void slaveCreator(int amount, int *pidArray, int pipefdArray[][2], int parentfd[2], char * files[]);

int main (int argc, char *argv[]){
    if(argc <= 1){
        printf("Input at least one file\n");
        return 0;
    }
    int filenum = argc - 1;
    int amount;
    if (filenum <= 8)
        amount = filenum;
    else 
        amount = 8;

    

    pid_t pidArray[amount];
    int pipefdArray[amount][2];
    int parentfd[2];
    if (pipe(parentfd) == -1){
        perror("Error occured while excecuting pipe");
        exit(1);
    }

    int aux = parentfd[0];
    if(dup2(parentfd[0], 0) == -1){
        perror("Error duping pipe");
        exit(1);
    }

    slaveCreator(amount, pidArray, pipefdArray, parentfd, argv);
    close(aux);
    close(parentfd[1]);
    //Mientras queden archivos
    char str[256];
    for (int i = 0; i < amount; i++) {// while me quedan archivos
        read(0, str, 256);
        printf("string pid: %s\n", str);
        pid_t pid = atoi(str);
        printf("int pid: %d\n", pid);
        int found = 0;
        int index;
        for (int j = 0; j < amount && found != 1; j++) {
            if (pidArray[j] == pid){
                found = 1;
                index = j;
            }
        }
        if (found) {
            write(pipefdArray[index][1], "-", sizeof("-"));// write nombre de siguiente archivo
        }
    }

    for (int i = 0; i < amount; i++) {
        //write(pipefdArray[i][1], "-", sizeof("-"));
        waitpid(pidArray[i], NULL, 0);
    }
    
    printf("PARENT FINISHED!\n");

    return 0;
}

void slaveCreator(int amount, pid_t *pidArray, int pipefdArray[][2], int parentfd[2], char * files[]){
    int i, j = 1;

    for (i = 0; i < amount; i++, j++) {
        
        if (pipe(pipefdArray[i]) == -1){
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
            if(dup2(pipefdArray[i][0], 0) == -1 || dup2(parentfd[1], 1) == -1) {
                perror("Error duping pipe");
                exit(1);
            }
            close(aux1);
            close(aux2);
        
            char * args[] = {"./slave", NULL};
            if ( execv(args[0], args) == -1 ) {
                perror("Error occured while executing slave process\n");
                exit(1);
            }
        }

        
        printf("Parent writing %s to pid: %d\n", files[j], pidArray[i]);
        write(pipefdArray[i][1], files[j], strlen(files[j])+1);
    }

}

