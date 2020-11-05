#include <unistd.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include "tar.h"
#include "functionInTar.h"

/* Print the values of environment variable PWD */
void pwd (){
    char * path = getPWD();
    strcat(path,"\n");
    print(path);
    free(path);
}

/*
int main (){
    char * test = malloc (sizeof(char) * 100);
    //strcpy (test,"a/b.tar/c/toto");
    //strcpy (test,"a/b/c/toto");
    if (isInTar(test) == 1)
        print("This Path is in a Tar");
    else
        print("This Path is NOT in a Tar"); 
    dividePathWithTar(strcat(getPWD(),"/aaaa/bb/c.tar/dqdsqfz"));
    print(relatifToAbsolute("aaa/bbb/c.tar/ddd/toto"));
    printf("Final : %s\n",pathWithoutPoint(relatifToAbsolute("bonjour/detruit/../detruit/../ree/./bonsoir")));
}
*/

