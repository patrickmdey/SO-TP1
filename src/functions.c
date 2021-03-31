// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>

void getFileChar(char *buffer, int maxsize){
    char c;
    int i = 0;
    while((c = getchar()) != 0 && i < maxsize) {
        buffer[i++] = c;
    }
    buffer[i] = 0;
}

#endif