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

 
 struct posix_header * searchFileInTar (char * pathName, char * archive){

    int fd = open(archive, O_RDONLY);
    if(fd < 0)
        print("bug sur l'ouverture");
    struct posix_header * p = (struct posix_header *)malloc(sizeof(struct posix_header));
    while(read(fd, p, 512) > 0){
        if(strcmp (p -> name, pathName) == 0){

            int numberBlock = 0;
            sscanf(p -> size ,"%o", &numberBlock);
            numberBlock = (numberBlock + 512 -1) /512;
            char * message =(char *) malloc (sizeof(char) * 512 * numberBlock);
            read (fd, message, sizeof(char) * 512 * numberBlock);
            print(message);
            return p;
        }
        else{
            int numberBlock = 0;
            sscanf(p -> size ,"%o", &numberBlock);
            numberBlock = (numberBlock + 512 -1) /512;
            for(int i = 0; i<numberBlock ; i++){
                int tmp1 = read (fd, p, 512);

                if( tmp1 < 0)
                    print("erreur de read");
            }
        }
    }
    print("PAS TROUVER");
    return NULL;
}

/*int main(int argc, char * argv[]){
    return 0;
}*/