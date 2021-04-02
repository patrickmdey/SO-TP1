#ifndef SLAVE_H
#define SLAVE_H

#define STRING_SIZE 256

void runMinisat(const char* path);
int readStr(FILE * fd, char * buff, int maxsize);

#endif