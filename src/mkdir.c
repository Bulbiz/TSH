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

