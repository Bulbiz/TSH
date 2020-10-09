#include <unistd.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define SIZE 20000

int isInTar();
char * pwd ();
void setUpForTarEnter();
void setUpForTarExit();

//FIXME : change printf with the personal print
char * pwd (){
    char * path = (char *) malloc (sizeof (char) * SIZE);
    char * pathTar = getenv("PWDforTar");
    (isInTar() == TRUE) ? printf("%s\n",strcat(getcwd(path, SIZE), pathTar)) : printf ("%s\n",getcwd(path, SIZE));
    free(path);
}

int isInTar (){
    if (getenv("PWDforTar") == NULL )
        return FALSE;
    else
        return TRUE;
    
}

/*Set up when entering a tar */
void setUpForTarEnter (){
    if(putenv ("PWDforTar") != 0)
        perror("PWDforTar");
    if(setenv ("PWDforTar","/",1) != 0)
        perror("PWDforTar");
}

/*Set up when exit a tar */
void setUpForTarExit (){
    if(unsetenv ("PWDforTar") != 0)
        perror("Environment variable PWDforTar could'nt been destroyed");
}
