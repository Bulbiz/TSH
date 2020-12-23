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

/* Execute the command mkdir inside a tarball */
int mkdirInTar(char * archive, char * path){
    int fd = openArchive(archive, O_RDWR);
    struct posix_header * buf = malloc (BLOCKSIZE);
    
    if(searchFile(fd,buf,path) == 0){
        free(buf);
        close(fd);
        print("Mkdir : Dossier existe !");
        return -1;
    }

    replaceCurseurToStart(fd);
    passArchive(fd);
    struct posix_header * headerFolder = createHeaderFolder (path);
    write(fd, headerFolder, BLOCKSIZE);
    
    free(buf);
    free(headerFolder);
    close(fd);
    return 0;
}
/* Divide the path for the execution of mkdir inside a tarball */
int myMkdir_aux (char * path){
    char ** pathInTar = (char **) dividePathWithTar (path);
    return mkdirInTar(pathInTar[0], pathInTar[1]);
}

/* Execute mkdir */
int myMkdir (char ** argv) {
    if (getArgc(argv) < 2){
        print("Not enough argument\n");
        return -1;
    }

    argv[1] = addSlash (argv[1]);

    if (!isArchiveRacine (getRepertoryRepertory(argv[1])) == 0 && fileExist(getRepertoryRepertory(argv[1])) != 0){
        print ("Chemin non valide !\n");
        return -1;
    }

    if (fileExist (argv[1]) == 0){
        print ("Le dossier existe déja, impossible de le créer !\n");
        return -1;
    }

    if(isInTar(argv[1]) == 0)
        myMkdir_aux (argv[1]);
    else
        executeCommandExterne(argv);
    return 0;
    
}