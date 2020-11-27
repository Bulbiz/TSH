#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "tar.h"
#include "functionInTar.h"
#include "pathTreatement.h"

int setPWD (char * path){
    strcpy(cwd,path);
    free(path);
    return 0;
}

int cdAux (char * path){
    char ** traitedPath = dividePathWithTar(path);
    int fd = openArchive(traitedPath[0],O_RDONLY);
    if(fd < 0) {return -1;}

    if(strcmp(traitedPath[1],"") == 0){
        close(fd);
        return setPWD (path);
    }
    
    struct posix_header * buf = malloc (BLOCKSIZE);
    if ((searchFile(fd,buf,traitedPath[1]) == 0) && (buf -> typeflag = '5')){
        free(buf);
        close(fd);
        return setPWD (path);
    } else {
        free(buf);
        close(fd);
        return -1;
    }

}

void cd (char * path){
    strcat(path,"/");
    if (isInTar(path) == 0){
        if(cdAux(path) == -1)
            print("CD : Déplacement échoué\n");
    }else{
        if(chdir(path) < 0)
            perror("CD");
        getcwd(cwd,SIZE);
    }
}

/*
int main (){
    printf("Avant : %s\n",getenv("PWD"));
    cd("archive.tar/rep/");
    printf("Après : %s\n",getenv("PWD"));
    cd(duplicate("/home/pholasa/Bureau/L3/Système d'exploitation/projet/shellfortarball/archive.tar/"));
    printf("Après2 : %s\n",getenv("PWD"));
}*/

