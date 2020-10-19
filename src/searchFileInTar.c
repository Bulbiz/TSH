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
#include "print.h"
#include "functionInTar.h"
 
 struct posix_header * searchFileInTar (char * pathname, char * archive){

    int fd = openArchive (pathname, O_RDONLY);
    struct posix_header p;
    while(readHeader (fd, p) > 0){
        if(strcmp (p.name, pathname) == 0){
            print(getContent (p,fd));
            return &p;
        }
        else
            passContent (p,fd);    
    }
    print("PAS TROUVER");
    return NULL;
}

/*int main(int argc, char * argv[]){
    return 0;
}*/