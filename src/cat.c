#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//cat without path
int catOnly (){
    char * save = (char *) malloc (sizeof(char)* 512);
    ssize_t sizeMessage = read(STDIN_FILENO, save, 512);
    if( sizeMessage < 0){
        printf("bug sur la lecture");
        return -1;
    }

    while(sizeMessage > 0){
        if( write(STDOUT_FILENO , save, sizeMessage) < 0){
            printf("bug sur l'écriture");
            return -1;
        }

        sizeMessage = read(STDIN_FILENO, save, 512);
    }
    return 0;
}


int main(int argc, char * argv[]){
    catOnly();
    return 0;
}