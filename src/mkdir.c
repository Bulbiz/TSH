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

//FIXME : à completer
int mkdirInTar(char * archive, char * path){


    return 0;
}

void myMkdir (char * path){
    char ** pathInTar = (char **) dividePathWithTar (path);
    mkdirInTar(pathInTar[0], pathInTar[1]);
}