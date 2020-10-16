#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void print (char * message){
    if (write (STDIN_FILENO,message,strlen(message)) == -1)
        perror("Print :");
}

