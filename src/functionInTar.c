#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "tar.h"
#include "print.h"

#define BLOCKSIZE 512

int openArchive (char * pathname, int flags){
    int tmp = open(pathname, flags);
    if(tmp < 0)
        perror("open");
    return tmp;
}

int readHeader (int fd, struct posix_header * buffer){
    int tmp = read(fd, buffer, 512);
    if(tmp < 0)
        perror("read");
    return tmp;
}

char * getContent (struct posix_header * header, int fd){
    int numberBlock = 0;
    sscanf(header -> size ,"%o", &numberBlock);
    numberBlock = (numberBlock + 512 -1) /512;
    char * message =(char *) malloc (sizeof(char) * 512 * numberBlock);
    read (fd, message, sizeof(char) * 512 * numberBlock);
    return message;
}

void passContent (struct posix_header * header, int fd){
    int numberBlock = 0;
    sscanf(header -> size ,"%o", &numberBlock);
    numberBlock = (numberBlock + 512 -1) /512;
    read (fd, header, sizeof(char) * 512 * numberBlock);
}

