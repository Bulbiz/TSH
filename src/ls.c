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

/* FIXME : pour l'instant on va le faire que sur un argument, il faudra modifier pour plusieur argument */
void ls (char * path) {
    
    char ** division = dividePathWithTar (path);
    int fdArchive = openArchive(division[0],O_RDONLY);
    struct posix_header * buf = malloc (BLOCKSIZE);
        
    char repertoire [sizeof(char) * (strlen(division[1]) + 1)];
    strcpy (repertoire,division[1]);

    if (strlen(division[1]) > 0)
        strcat (repertoire,"/");

    replaceCurseurToStart (fdArchive);
    while(getHeader(fdArchive,buf) == 0 ){
        if (isInRepertory(repertoire,buf -> name) == 0){
            print(buf -> name + strlen(repertoire)); 
            print("   ");
        }
    }
    
    print("\n");
}
