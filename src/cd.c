#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "print.h"
#include "tar.h"
#include "functionInTar.h"
#include "pwd.h"
int cdAux (char * path){
    printf("IN : %s\n", path);
    char ** traitedPath = dividePathWithTar(path);
    int fd = openArchive(traitedPath[0],O_RDONLY);
    if(fd < 0) {return -1;}
    struct posix_header * buf = malloc (BLOCKSIZE);
    if ((searchFile(fd,buf,traitedPath[1]) == 0) && (buf -> typeflag = '5'))
        setenv("PWD",path,1);
    else 
        return -1;
    close(fd);
    return 0;
}

void cd (char * arg){
    char * path = (isAbsolute(arg) == 0)? arg : relatifToAbsolute(arg);
    char * newpath = traitementOfPath(path);
    strcat(newpath,"/");
    if(cdAux(newpath) == -1)
        print("cd : Cannot enter ! HELP!\n");
}

/*
int main (){
    printf("Avant : %s\n",getenv("PWD"));
    cd("archive.tar/rep");
    printf("Après : %s\n",getenv("PWD"));
}
*/
