#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "tar.h"
#include "functionInTar.h"

int setPWD (char * path, int fd){
    setenv("PWD",path,1);
    close(fd);
    return 0;
}

int cdAux (char * path){
    char ** traitedPath = dividePathWithTar(path);
    int fd = openArchive(traitedPath[0],O_RDONLY);
    if(fd < 0) {return -1;}

    if(strcmp(traitedPath[1],"") == 0)
        return setPWD (path,fd);
    
    struct posix_header * buf = malloc (BLOCKSIZE);
    if ((searchFile(fd,buf,traitedPath[1]) == 0) && (buf -> typeflag = '5')){
        return setPWD (path,fd);
    } else {
        close(fd);
        return -1;
    }

}

void cd (char * arg){
    char * path = pathTreated(arg);
    strcat(path,"/");
    if(cdAux(path) == -1)
        print("cd : Cannot enter ! HELP!\n");
}

/*
int main (){
    printf("Avant : %s\n",getenv("PWD"));
    cd("archive.tar/rep/");
    printf("Après : %s\n",getenv("PWD"));
    cd(duplicate("/home/pholasa/Bureau/L3/Système d'exploitation/projet/shellfortarball/archive.tar/"));
    printf("Après2 : %s\n",getenv("PWD"));
}*/

