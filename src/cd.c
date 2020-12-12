#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "tar.h"
#include "functionInTar.h"
#include "pathTreatement.h"

/*copy the path in the global variable cwd*/
int setPWD (char * path){
    strcpy(cwd,path);
    free(path);
    return 0;
}

/*change the path in the global variable cwd in case of cd in tar if the path is a directory*/
int cdInTar (char * path){
    char ** traitedPath = dividePathWithTar(path);
    int fd = openArchive(traitedPath[0],O_RDONLY);
    if(fd < 0) {return -1;}

    if(strcmp(traitedPath[1],"") == 0){
        close(fd);
        return setPWD (path);
    }
    
    struct posix_header * buf = malloc (BLOCKSIZE);
    if ((searchFile(fd,buf,traitedPath[1]) == 0) && (buf -> typeflag = '5')){
        free(buf);
        close(fd);
        return setPWD (path);
    } else {
        free(buf);
        close(fd);
        return -1;
    }

}

/*add "/" if the path don't have at the end, and if the path is a folder update the 
global variable cwd */
void cd_aux (char * path){
    if(path[strlen(path) - 1] != '/')
        strcat(path,"/");
        
    if (isInTar(path) == 0){
        if(cdInTar(path) == -1)
            print("CD : Déplacement échoué\n");
    }else{
        if(chdir(path) < 0)
            perror("CD");
        getcwd(cwd,SIZE);
    }
}
/*verification argv and do cd_aux*/
void cd (char ** argv) {
    if (getArgc(argv) != 2)
        print("Trop d'argument !\n");
    else
        cd_aux(argv[1]);
}


