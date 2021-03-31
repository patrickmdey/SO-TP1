// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define _BSD_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>

#include "functions.h"

#define STRING_SIZE 256

void runMinisat(const char* path);

int main(int argc, const char* argv[]) {
    char file[STRING_SIZE] = { 0 };
    
    getFileChar(file, STRING_SIZE);
    while (file[0] != 0) {
        runMinisat(file);
        getFileChar(file, STRING_SIZE);
    }

    return 0;
}

int readStr(FILE * fd, char * buff, int maxsize) {
	int i = 0;
	char c;
	while ((c = fgetc(fd)) != 0 && (unsigned int) c != (unsigned int)EOF && i < maxsize) {
		buff[i++] = c;
	}
	buff[i] = 0;
	return i;	
}

void runMinisat(const char* path) {
    char execute[SIZE] = { 0 };
    strcpy(execute, "minisat ");
    strcat(execute, path);
    strcat(execute, " | /bin/grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\""); //2>/dev/null
    FILE * ret = popen(execute, "r");
    if (ret == NULL) {
        perror("Error occurred while executing minisat");  
        exit(1);
    }

    char str[SIZE/2];
    readStr(ret, str, SIZE);
    char out[SIZE];
    int len = sprintf(out, "pid: %d\nfile: %s\n%s\n", getpid(), path, str);
    write(1, out, len + 1);
    pclose(ret);
}


