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
#include "functionInTar.h"
#include "pwd.h"
 
void cat (char * path){
    if(isInTar (path)){
        char ** pathInTar =  dividePathWithTar (path);
        catInTar(pathInTar[0], pathInTar[1]);
    }else{
        catOusideTar(path);
    }
}

void catInTar(char * archive, char * path){
    int fd = openArchive (archive, O_RDONLY);
    struct posix_header * p = malloc (512);
    while(readHeader (fd, p) > 0){
        if(strcmp (p -> name, path) == 0){
            print(getContent (fd,p));
        }
        else
            passContent (fd,p);    
    }
    print("fichier introuvable");
}

void catOusideTar(char * path){
     
}

int main(int argc, char * argv[]){
    return 0;
}