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

int cpTarInTar(char * archive, char * path, char * destination){
    return 0;
}

int cpTarInOutsideTar(char * archive, char * path, char * destination){
    return 0;
}

int cpOutsideTarInTar(char * archive, char * path, char * destination){
    return 0;
}

void cp1 (char * path, char * destination){
    char ** pathInTar = (char **) dividePathWithTar (path);
    char ** destinationInTar = (char **) dividePathWithTar (destination);
    cpTarInTar(pathInTar[0], pathInTar[1], destinationInTar[1]);
}

void cp2 (char * path, char * destination){
    char ** pathInTar = (char **) dividePathWithTar (path);
    char ** destinationInTar = (char **) dividePathWithTar (destination);
    cpTarInOutsideTar(pathInTar[0], pathInTar[1], destinationInTar[1]);
}

void cp3 (char * path, char * destination){
    char ** pathInTar = (char **) dividePathWithTar (path);
    char ** destinationInTar = (char **) dividePathWithTar (destination);
    cpOutsideTarInTar(destinationInTar[0], pathInTar[1], destinationInTar[1]);
}