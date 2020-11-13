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

int rmInTar(char * archive, char * path){
    int fd = openArchive (archive, O_RDONLY);
    size_t size = getSizeAfterFile (path, fd);
    struct posix_header * buf = malloc (512);
    size_t fileSize = searchFileSize (fd, buf, path);
    replaceCurseurToStart (fd);

    char * contentAfterFile = (char *) malloc (sizeof(char)* size);
    contentAfterFile = getContentUntilPathFile(path, fd, size);

    searchFile(fd, buf, path);
    char * tmp0 = (char *) malloc (sizeof(char)* size + (fileSize));
    memset(tmp0, 0, size + fileSize);
    write(fd, tmp0, size + fileSize);

    replaceCurseurToStart (fd);
    searchFile(fd, buf, path);
    write (fd, contentAfterFile, size);
    
    free(path);
    free(archive);
    free(contentAfterFile);
    free(buf);
    free(tmp0);
    close(fd);
    return -1;
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

/*int main(int argc, char * argv[]){
    return 0;
}*/