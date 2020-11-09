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

#define TRUE 1
#define LIMIT 20000


char buffer [LIMIT];

/* Get the user input, clean the buffer every time a new input is entered */
void userInput (){
    memset(buffer,'\0',LIMIT);
    read(STDIN_FILENO, buffer, LIMIT);
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
        if(command[i] == ' ')
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
            waitpid (-1,NULL,WNOHANG);
        break;
    }
}

//main function that executes the shell
void shell(){
    while(TRUE){
        userInput ();  //we need a function who separate the option
        char * command = "";
        char * options = "";

        if(strcmp (command,"cd") == 0){

        }else if(strcmp (command,"pwd") == 0){

        }else if(strcmp (command,"mkdir") == 0){

        }else if(strcmp (command,"rmdir") == 0){
            
        }else if(strcmp (command,"mv") == 0){
            
        }else if(strcmp (command,"cp") == 0){
            
        }else if(strcmp (command,"rm") == 0){
            
        }else if(strcmp (command,"ls") == 0){
            
        }else if(strcmp (command,"cat") == 0){
            
        }else if(strcmp (command,"exit") == 0){
            exit(0);
        }else{
            print("commande inconnue");
        }
    }
}





int main (){
    userInput ();
    char ** argv = getArgument (buffer);
    int i = 0;
    while (argv[i]!= NULL){
        printf("Argument : %s\n",argv[i++]);
    }
    return 0;
}