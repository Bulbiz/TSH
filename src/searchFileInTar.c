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
 
 struct posix_header * searchFileInTar (char * archive, char * pathname){

    int fd = openArchive (archive, O_RDONLY);
    struct posix_header * p = malloc (512);
    while(readHeader (fd, p) > 0){
        if(strcmp (p -> name, pathname) == 0){
            print(getContent (fd,p));
            return p;
        }
        else
            passContent (fd,p);    
    }
    print("PAS TROUVER");
    return NULL;
}

/*int main(int argc, char * argv[]){
    return 0;
}*/