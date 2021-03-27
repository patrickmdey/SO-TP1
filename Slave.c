#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

static int itoa(int n, char s[]);
static void reverse(char str[]);

void runMinisat(const char* path);

int main(int argc, const char* argv[]) {
    char file[256] = { 0 };
    read(0, file, 256);
    while (file[0] != '-') {
        //solve file filename
        //printf("%s", file);
        //runMinisat(file);

        char pidString[10];

        int len = itoa(getpid(), pidString);
        write(1, pidString, len + 1);
        read(0, file, 256);
    }
    return 0;
}

void runMinisat(const char* path) {
    int minisatPipeFd[2];
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
    close(minisatPipeFd[1]);
}

static int itoa(int n, char s[]) {
    int i;

    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    s[i] = '\0';
    reverse(s);
    return i;
}

static void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}