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
#include "pwd.h"
#include "functionInTar.h"
#include "pathTreatement.h"

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

    free(path);
    free(archive);
    free(p);
    close(fd);

    print("fichier introuvable\n");
    return -1;
}

void cat (char * path){
    char ** pathInTar = (char **) dividePathWithTar (path);
    catInTar(pathInTar[0], pathInTar[1]);
}

/*int main(int argc, char * argv[]){
    cat(relatifToAbsolute ("test/titi"));
    return 0;
}*/