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

//FIXME : manque la vérification si le path est un fichier ou pas
int rmInTar(char * archive, char * path){
    int fd = openArchive (archive, O_RDWR);

    size_t size = getSizeAfterFile (path, fd);
    printf("Size : %ld", size);
    
    replaceCurseurToStart (fd);

    struct posix_header * buf = malloc (512);
    size_t fileSize = searchFileSize (fd, buf, path);
    printf("FileSize : %ld", fileSize);

    replaceCurseurToStart (fd);

    char * contentAfterFile = (char *) malloc (sizeof(char)* size);
    contentAfterFile = getContentUntilPathFile(path, fd, size);
    printf("Content after file : %s ", contentAfterFile);
    
    searchFile(fd, buf, path);
    lseek(fd, -BLOCKSIZE, SEEK_CUR);
    char * tmp0 = (char *) malloc (sizeof(char)* (size + fileSize));
    memset(tmp0, 0, size + fileSize);
    write(fd, tmp0, size + fileSize);

    passArchive(fd);
    write (fd, contentAfterFile, size);
    
    /*free(path);
    free(archive);
    free(contentAfterFile);
    free(buf);
    free(tmp0);
    close(fd);*/
    return 0;
}

/*void catOutsideTar(char * path){
    int pid = fork();
    switch(pid){
        case -1 : perror ("cat");break;
        case 0 : execlp("cat", "cat", path, NULL); break;
        default : break;
    }    
}

void cat (char * path){
    if(isInTar(path) == 0){
        char ** pathInTar = (char **) dividePathWithTar (path);
        catInTar(pathInTar[0], pathInTar[1]);
    }else{
        catOutsideTar(path);
    }
}*/

int main(int argc, char * argv[]){
    rmInTar("archive.tar","rep/rep2/toto");
    return 0;
}