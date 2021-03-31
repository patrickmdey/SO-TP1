// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>

#include "shared_memory.h"

int main(int argc, char * argv[]) {
    int shm_size;
	if (argc == 1) {
		//No recibi por linea de comandos leo de entrada estandard
        read(0, &shm_size, sizeof(int));
	} 
    else if (argc == 2){
        shm_size = atoi(argv[1]);
	} else {
        printf("Invalid arguments");
        return 0;
    }

    if (shm_size == 0) {
        perror("Invalid memory size");
        exit(1);
    }
    
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0);
    if(shm_fd == -1){
        perror("Error occured while trying to open the shared memory");
        exit(1);
    }

    sem_t * sem = sem_open(SEM_NAME, O_RDWR);
    if(sem == SEM_FAILED) {
        perror("Error occured while trying to initialize a semaphore");
        exit(1);
    }

    //int size = atoi(argv[2]);
    /*if(ftruncate(buff_fd, size) == -1){
        perror("Error occured while trying excecuting ftruncate");
        exit(1);
    }*/

    char * shmem = (char *) mmap(NULL, shm_size, PROT_READ, MAP_SHARED, shm_fd, 0);
    if(shmem == (void*) -1){
        perror("Error occured while trying to map the shared memory block");
        exit(1);
    }
    

    //int * data = (int *) mapped;

    int done = 0;
    while (done == 0) {
    	//printf("Esperando semaforo\n");
    	sem_wait(sem); // Me bloqueo hasta que me permitan leer
    	//printf("Semaforo termino\n");
        if (*shmem == 0) // EOF
            done = 1;
        else
    	    shmem += printf("%s", shmem) + 1;
    }

    sem_close(sem);
    munmap(shmem, shm_size);
    close(shm_fd);

    return 0;
}
