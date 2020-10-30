#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "tar.h"
#include "functionInTar.h"
#include "pwd.h"
#include <string.h>

int catAux (char * path){
    printf("IN : %s\n", path);
    char ** traitedPath = dividePathWithTar(path);
    int fd = openArchive(traitedPath[0],O_RDONLY);
    if(fd < 0) {return -1;}
    struct posix_header * buf = malloc (BLOCKSIZE);
    if ((searchFile(fd,buf,traitedPath[1]) == 0) && (buf -> typeflag = '5'))
        setenv("PWD",path,1);
    close(fd);
    return 0;
}

int main (){
    struct posix_header * buf = malloc (BLOCKSIZE);
    printf("%d\n",searchFile(openArchive("archive.tar",O_RDONLY),buf,"rep/rep2/"));
    printf("Avant : %s\n",getenv("PWD"));
    printf("%d\n", catAux(relatifToAbsolute("archive.tar/rep/")));
    printf("Après : %s\n",getenv("PWD"));
}
