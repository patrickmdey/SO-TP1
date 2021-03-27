#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>

int main(int argc, char * argv[]) {
    if (argc <= 2) {
        printf("Input direction\n");
        return 0;
    }
    
    int buff_fd = shm_open(argv[1], O_RDONLY, 0);
    if(buff_fd == -1){
        perror("Error occured while trying to open the shared memory");
        exit(1);
    }

    int size = atoi(argv[2]);
    if(ftruncate(buff_fd, size) == -1){
        perror("Error occured while trying excecuting ftruncate");
        exit(1);
    }

    void * mapped = mmap(NULL, size, PROT_READ, MAP_SHARED, buff_fd, 0);
    if(mapped == (void*) -1){
        perror("Error occured while trying to map the shared memory block");
        exit(1);
    }

    sem_t *sem = sem_open("BufferSemaphore", 0);
    

    //int * data = (int *) mapped;

    sem_close(sem);
    munmap(mapped, size);
    shm_unlink(argv[1]);
    close(buff_fd);
    return 0;
}
