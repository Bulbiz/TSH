#include "pathTreatement.h"
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

int mkdirInTar(char * archive, char * path){
    int fd = openArchive(archive, O_RDWR);
    passArchive(fd);
    struct posix_header * headerFolder = createHeaderFolder (path);
    write(fd, headerFolder, BLOCKSIZE);
    
    free(headerFolder);
    close(fd);
    return 0;
}

void myMkdir_aux (char * path){
    char ** pathInTar = (char **) dividePathWithTar (path);
    mkdirInTar(pathInTar[0], pathInTar[1]);
}

void myMkdir (char ** argv) {
    if(isInTar(argv[1]) == 0)
        myMkdir_aux (argv[1]);
    else
        executeCommandExterne(argv);
}