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

//give the number for the name after the last '/'
int fileName (char * pathName){
    int i = strlen(pathName) -2;
    while(pathName[i] != '/')
        i--;
    return i;
}

int cpTarInTar(char * archive, char * path, char * destination){
    int fd = openArchive (archive, O_RDWR);
    struct posix_header * copyHeader = malloc (BLOCKSIZE);
    if(searchFile (fd, copyHeader, path) == -1){
        perror("fichier inexistant");
        return -1;
    }
    
    memset(copyHeader -> name, 0, 100);
    memcpy(copyHeader -> name, destination, (strlen(destination)));
    set_checksum(copyHeader);
    if(!check_checksum(copyHeader)){
        perror("Checksum :");
        return -1;
    }

    int filesize;
    sscanf(copyHeader -> size, "%o" , &filesize);
    char * copyContent = malloc (sizeof(char) * filesize);
    read (fd, copyContent, filesize);
    passArchive(fd);

    write(fd, copyHeader, BLOCKSIZE);
    write(fd, copyContent, filesize);

    free(copyHeader);
    free(copyContent);
    close(fd);
    return 0;
}

int cpTarInOutsideTar(char * archive, char * path, char * destination){
    int fd = openArchive (archive, O_RDONLY);

    struct posix_header * copyHeader = malloc (BLOCKSIZE);
    if(searchFile (fd, copyHeader, path) == -1){
        perror("fichier inexistant");
        return -1;
    }

    int filesize;
    sscanf(copyHeader -> size, "%o" , &filesize);

    char * copyContent = malloc (sizeof(char) * filesize);

    read (fd, copyContent, filesize);

    int fdFile = open(destination, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    
    if(write(fdFile, copyContent, strlen(copyContent))<0){
        perror("erreur sur l'écriture");
        return -1;
    }

    free(copyHeader);
    free(copyContent);
    close(fd);
    close(fdFile);
    return 0;
}

int cpOutsideTarInTar(char * archive, char * path, char * destination){
    int fd = openArchive (archive, O_RDWR);
    int fdFile = open(path, O_RDONLY);
    copyFileToTar (fd, fdFile, destination);
    close(fd);
    close(fdFile);
    return 0;
}

int cp1 (char * path, char * destination){
    char ** pathInTar = (char **) dividePathWithTar (path);
    char ** destinationInTar = (char **) dividePathWithTar (destination);
    return cpTarInTar(pathInTar[0], pathInTar[1], destinationInTar[1]);
}

int cp2 (char * path, char * destination){
    char ** pathInTar = (char **) dividePathWithTar (path);
    return cpTarInOutsideTar(pathInTar[0], pathInTar[1], destination);
}

int cp3 (char * path, char * destination){
    char ** destinationInTar = (char **) dividePathWithTar (destination);
    return cpOutsideTarInTar(destinationInTar[0], path, destinationInTar[1]);
}


int cp (char ** argv){
    if (getArgc(argv) > 3 && getArgc(argv) < 3)
        print("Trop d'arguments ou pas assez d'arguments!\n");

    int pathName = isInTar(argv[1]);
    int destination = isInTar(argv[2]);

    if(isARepertory(argv[2]) == 0){
        char * buf = malloc ( sizeof(char) * (strlen(argv[2]) + strlen(argv[1] + fileName (argv[1]))));
        sprintf(buf,"%s%s", argv[2], argv[1] + fileName (argv[1]) + 1);
        argv[2] = buf;
    }

    if(pathName == 0 && destination == 0)
        return cp1(argv[1], argv[2]);
    else if (pathName == 0 && destination == -1)
        return cp2(argv[1], argv[2]);
    else if (pathName == -1 && destination == 0) 
        return cp3(argv[1], argv[2]);
    else{
        executeCommandExterne(argv);
        return 0;
    }
}