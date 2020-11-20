#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "tar.h"
#include "functionInTar.h"
#include "pathTreatement.h"


int isInRepertory (char * repertory, char * filename){
    return (
        strlen (repertory) < strlen(filename) && 
        strncmp(repertory,filename,strlen(repertory)) == 0 && 

        (numberOfSlash (filename) == numberOfSlash(repertory)  || 
        (numberOfSlash (filename) == numberOfSlash(repertory) + 1 && filename[strlen(filename) - 1] == '/'))) ? 0 : -1;

}
/* FIXME : pour l'instant on va le faire que sur un argument, il faudra modifier pour plusieur argument */
int ls (char ** argv) {
    
    char * path = pathTreated(argv[1]);
    
    if (isInTar(path) == 0){
        char ** division = dividePathWithTar (path);
        int fdArchive = openArchive(division[0],O_RDONLY);
        struct posix_header * buf = malloc (BLOCKSIZE);
        
        char * repertoire = malloc (sizeof(char) * (strlen(division[1]) + 3));
        strcat (repertoire,division[1]); ////
        strcat (repertoire,"/");

        replaceCurseurToStart (fdArchive);
        while(getHeader(fdArchive,buf) == 0 ){
            if (isInRepertory(repertoire,buf -> name) == 0){
                print(buf -> name + strlen(repertoire)); 
                print("   \n");
            }
        }
    }
//    printf ("FIN\nResultat : %d\n", isInRepertory(argv[1],argv[2]));
    return 0;
}
/*
int ls (char * archive, char * path){

}
*/
/*
int lsPath(const char *path) {
    DIR *d = opendir(path);
    if(!d) {
        perror("opendir");
        return -1;
    }

    struct dirent *dir;
    while((dir = readdir(d))) {
        printf("%s\n", dir->d_name);
    }
    closedir(d);

    return 0;
}

int ls(int argc, char *argv[]) {
    int returnValue = 0;

    if(argc == 1) {
        if(lsPath(".") != 0) {
            return -1;
        }
    } else {
        for(int i = 1; i < argc; i++) {
            printf("%s:\n", argv[i]);
            if(lsPath(argv[i]) != 0) {
                returnValue = -1;
            }
            printf("\n");
        }
    }

    return returnValue;
}
*/