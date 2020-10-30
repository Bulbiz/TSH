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
#include "pwd.h"
#include "functionInTar.h"

int catInTar(char * archive, char * path){
    int fd = openArchive (archive, O_RDONLY);
    struct posix_header * p = malloc (512);
    while(readHeader (fd, p) > 0){
        if(strcmp (p -> name, path) == 0){
            print(getContent (fd,p));
            return 0;
        }
        else
            passContent (fd,p);    
    }
    print("fichier introuvable\n");
    return -1;
}

void catOutsideTar(char * path){
    int pid = fork();
    switch(pid){
        case -1 : perror ("cat");break;
        case 0 : execlp("cat", "cat", path, NULL); break;
        default : break;
    }    
}

void cat (char * path){
    if(isInTar(path) == 1){
        char ** pathInTar = (char **) dividePathWithTar (path);
        catInTar(pathInTar[0], pathInTar[1]);
    }else{
        catOutsideTar(path);
    }
}

/*int main(int argc, char * argv[]){
    cat(relatifToAbsolute ("test/titi"));
    return 0;
}*/