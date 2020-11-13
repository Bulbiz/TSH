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

//return 0 is there is file in the folder else -1
int fileInFolderInTar (char * archive, char * path){
    int fd = openArchive (archive, O_RDONLY);
    struct posix_header * buf = malloc (512);
    while (getHeader(fd,buf) == 0){
        if(strstr(buf->name, path + '/') > 0)
            return 0;
        else
            passContent(fd, buf);
    }
    return -1;
}   

//FIXME : manque la vérification si le path est un dossier ou pas
int rmdirInTar(char * archive, char * path){
    if(fileInFolderInTar(archive, path) == -1){
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
        return 0;
    }else{
        print("action impossible, il reste des fichiers dans le dossier");
        return -1;
    }
}

