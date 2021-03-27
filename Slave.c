#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include "functions.h"

#define STRING_SIZE 256

//static int itoa(int n, char s[]);
//static void reverse(char str[]);

void runMinisat(const char* path);

int main(int argc, const char* argv[]) {
    char file[STRING_SIZE] = { 0 };
    //read(0, file, 256);
    getFileChar(file, STRING_SIZE);
    while (file[0] != '-') {
        //solve file filename
        runMinisat(file);

        //read(0, file, 256);
        getFileChar(file, STRING_SIZE);
        //printf("child recieved: %s\n", file);
    }
    return 0;
}

int readStr(FILE * fd, char * buff, int maxsize) {
	int i = 0;
	char c;
	while ((c = fgetc(fd)) != 0 && c != EOF && i < maxsize) {
		buff[i++] = c;
	}
	buff[i] = 0;
	return i;	
}

void runMinisat(const char* path) {
    char execute[4096] = { 0 };
    strcpy(execute, "minisat ");
    strcat(execute, path);
    strcat(execute, " | /bin/grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\"");
    FILE * ret = popen(execute, "r");
    if (ret == NULL) {
        perror("Error executing minisat");  
        exit(1);
    }

    char str[4096];
    readStr(ret, str, 4096);
    char out[4096];
    int len = sprintf(out, "pid: %d\nfile: %s\n%s", getpid(), path, str);
    write(1, out, len + 1);
    pclose(ret);
    

    /*int minisatPipeFd[2];
    if (pipe(minisatPipeFd) == -1) {
        perror("Error occured while executing Pipe");
        exit(1);
    }
    int pid = fork();
    if (pid == 0) {
        int aux = minisatPipeFd[1];
        dup2(aux, 1);
        close(minisatPipeFd[0]);
        close(aux);

        char execute[256] = { 0 };
        strcpy("./minisat", execute);
        strcat(execute, path);
        char* args[] = { execute, NULL };
        write(1, args[0], strlen(args[0]) + 1);
        // execv("")
    }
    int aux = minisatPipeFd[0];
    dup2(aux, 0);
    close(aux);
    close(minisatPipeFd[1]);*/
}

/*static int itoa(int n, char s[]) {
    int i;

    i = 0;
    do {       
        s[i++] = n % 10 + '0';   
    } while ((n /= 10) > 0);     
    s[i] = '\0';
    reverse(s);
    return i;
}*/

// static void reverse(char s[])
// {
//     int i, j;
//     char c;

//     for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
//         c = s[i];
//         s[i] = s[j];
//         s[j] = c;
//     }
// }

