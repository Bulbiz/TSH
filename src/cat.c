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

/* Do the command cat in the tar */
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

/* Convert the path for the execution of the command */
void cat_aux (char * path){
    if (fileExist(path) != 0){
        print("fichier introuvable\n");
        return;
    }
    char ** pathInTar = (char **) dividePathWithTar (path);
    catInTar(pathInTar[0], pathInTar[1]);
}

/* Execute the command cat */
void cat (char ** argv){
    if(getArgc (argv) == 1 || isInTar(argv[1]) == -1)                 //cat without argument or not in tar
        executeCommandExterne(argv);
    else
        cat_aux (argv[1]);
}
