#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>

void getFileChar(char *buffer, int maxsize){
    char c;
    int i = 0;
    while((c = getchar()) != 0 && c != EOF && i < maxsize) {
        buffer[i++] = c;
    }
    buffer[i] = 0;
}

#endif