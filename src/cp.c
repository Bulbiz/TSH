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
    int fd = openArchive (archive, O_RDWR);
    struct posix_header * copyHeader = malloc (BLOCKSIZE);
    if(searchFile (fd, copyHeader, path) == -1)
        perror("fichier inexistant");
    
    memset(copyHeader -> name, 0, 100);
    memcpy(copyHeader -> name, destination, (strlen(destination)));
    set_checksum(copyHeader);
    if(!check_checksum(copyHeader))
        perror("Checksum :");
    char * copyContent = malloc (sizeof(char) * (atoi(copyHeader -> size)));
    read (fd, copyContent, (atoi(copyHeader -> size)));
    passArchive(fd);

    write(fd, copyHeader, BLOCKSIZE);
    write(fd, copyContent, atoi(copyHeader -> size));

    free(copyHeader);
    free(copyContent);
    close(fd);
    return 0;
}

int cpTarInOutsideTar(char * archive, char * path, char * destination){
    //int fd = openArchive (archive, O_RDWR);

    return 0;
}

int cpOutsideTarInTar(char * archive, char * path, char * destination){
    //int fd = openArchive (archive, O_RDWR);

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