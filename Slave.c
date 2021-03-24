#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static int itoa(int n, char s[]);
static void reverse(char str[]);

int main(int argc, const char * argv[]) {
    char * str = malloc(sizeof(char) * 256);
    read(0, str, 256);
    while (str[0] != '-') {
        //printf("Input: %s\n", str);
        char pidString[10];
        int len = itoa(getpid(), pidString);
        write(1, pidString, len+1);
        //printf("%s\n", pidString);
        read(0, str, 256);   
    } 
    free(str);
    return 0;
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

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }