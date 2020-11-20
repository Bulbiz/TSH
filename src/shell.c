#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#include "tar.h"
#include "functionInTar.h"
#include "pathTreatement.h"

#include "cat.h"
#include "cd.h"
#include "cp.h"
#include "ls.h"
#include "mkdir.h"
#include "mv.h"
#include "pwd.h"
#include "rm.h"
#include "rmdir.h"

#define TRUE 1
#define LIMIT 20000


char buffer [LIMIT];

/* Get the user input, clean the buffer every time a new input is entered */
void userInput (){
    print("Votre commande $");
    memset(buffer,'\0',LIMIT); 
    int size = read(STDIN_FILENO, buffer, LIMIT);
    if(size < 0){
        perror("User Input:");
    }else{
        buffer[size - 1] = '\0'; // the last character (size -1) is a \n which is not wanted 
    }
}

int numberOfCommand (){
    int cmp = 0;
    for(int i=0; i<LIMIT; i++)
        if(buffer[i] == '|')
            cmp++;
    return cmp + 1;
}

/* cut the buffer with '|' */
char ** inputCutter (int * size) {
    *size = numberOfCommand();
    char ** command = (char **) malloc (sizeof(char *) * (*size));
    int pointer = 1;

    for(int i=0; i<LIMIT; i++){
        if(buffer[i] == '|'){
            command[pointer++] = (buffer + i + 1);
            buffer[i] = '\0';
        }
    }
    
    command[0] = buffer;
    return command;
}

/* Auxilary function */
int jumpArgument (char * command){
    int i = 0;
    while (command[i] != ' ' && command[i] != '\0'){
        i++;
    }
    return i;
}

/* Auxilary function */
int jumpSpace (char * command){
    int i = 0;
    while (command[i] == ' '){
        i++;
    }
    return i;
}

/* Count the number of word/argument */
int numberOfArgument (char * command){
    int cmp = 0;
    int i = 0;
    while(command[i]!= '\0'){
        if (command[i] == ' ')
            i += jumpSpace(command + i);
        else{
            cmp ++;
            i += jumpArgument(command + i);
        }
    }
    return cmp;
}

void replaceSpace (char * command){
    for (int i=0; command[i] != '\0'; i++)
        if(command[i] == ' ' )
            command[i] = '\0';
}

/* the last argument is NULL */
char ** getArgument (char * command){
    int pointer = 0;
    int i = 0;
    int nbOfArg = numberOfArgument (command);
    char ** argv = malloc (sizeof(char *) * (nbOfArg + 1));

    while(command[i]!= '\0'){
        if (command[i] == ' ')
            i += jumpSpace(command + i);
        else{
            argv[pointer++] = (command + i);
            i += jumpArgument(command + i);
        }
    }

    replaceSpace (command);
    argv[nbOfArg] = NULL;
    return argv;
}

int getArgc (char ** argv){
    int i = 0;
    while (argv[i] != NULL){
        i++;
    }
    return i;
}

/* FIXME : it seem like there is a small bug in the affichage ? not sure tho ... */
void executeCommandExterne (char ** argv){
    int child = fork ();

    switch (child){
        case -1 : perror ("Command execution"); break;
        case 0 ://child 
            if( execvp (argv[0],argv) == -1)
                perror ("Execution failure");
            break;
        default ://father
            wait(NULL);
        break;
    }
}

/*main function that executes the shell 
*/
void shell(){
    while(TRUE){
        userInput ();
        char ** argv = getArgument(buffer);

        if(strcmp (argv[0],"cd") == 0){
            if (getArgc(argv) > 2)
                print("Trop d'argument !\n");
            cd(argv[1]);

        }else if(strcmp (argv[0],"pwd") == 0){

            pwd();

        }else if(strcmp (argv[0],"mkdir") == 0){

            print("JE LANCE MKDIR!!! \n");

        }else if(strcmp (argv[0],"rmdir") == 0){

            print("JE LANCE RMDIR !!! \n");

        }else if(strcmp (argv[0],"mv") == 0){

            print("JE LANCE MV !!! \n");

        }else if(strcmp (argv[0],"cp") == 0){

            print("JE LANCE CP !!! \n");

        }else if(strcmp (argv[0],"rm") == 0){

            if(isInTar(argv[1]) == 0)
                rm (argv[1]);
            else
                executeCommandExterne(argv);

        }else if(strcmp (argv[0],"ls") == 0){

            ls(argv);

        }else if(strcmp (argv[0],"cat") == 0){

            if(isInTar(argv[1]) == 0)
                cat (argv[1]);
            else
                executeCommandExterne(argv);

        }else if(strcmp (argv[0],"exit") == 0){

            exit(0);

        }else{

            executeCommandExterne(argv);
            
        }
    }
}





int main (){
    cwd = malloc (SIZE);
    getcwd(cwd,SIZE);
    shell();
    return 0;
}
