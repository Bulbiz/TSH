#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


//cat without path
int catOnly (){
    char * save = (char *) malloc (sizeof(char)* 512);
    ssize_t sizeMessage = read(STDIN_FILENO, save, 512);
    if( sizeMessage < 0){
        printf("bug sur la lecture");
        return -1;
    }

    while(sizeMessage > 0){
        if( write(STDOUT_FILENO , save, sizeMessage) < 0){
            printf("bug sur l'écriture");
            return -1;
        }

        sizeMessage = read(STDIN_FILENO, save, 512);
    }
    return 0;
}

int catWithPath(char * path){
    int fd = open(path, O_RDONLY);
    char * save = (char *) malloc (sizeof(char)* 512);
    ssize_t sizeMessage = read(fd, save, 512);
    if( sizeMessage < 0){
        printf("bug sur la lecture");
        return -1;
    }

    while(sizeMessage > 0){
        if( write(STDOUT_FILENO , save, sizeMessage) < 0){
            printf("bug sur l'écriture");
            return -1;
        }

        sizeMessage = read(fd, save, 512);
    }
    return 0;
}

/*
int main(int argc, char * argv[]){
    //catOnly();
    //catWithPath("toto.txt");
    return 0;
}*/
