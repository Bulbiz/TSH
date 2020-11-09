#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tar.h"
#include "functionInTar.h"

#define TRUE 1
#define LIMIT 20000


char buffer [LIMIT];

void userInput (){
    for(int i=0; i<LIMIT; i++)
        buffer[i] = '\0';
    read(STDIN_FILENO, buffer, LIMIT);
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





int main (void){
    userInput ();
    print(buffer);
    userInput ();
    print(buffer);
    userInput ();
    print(buffer);
    return 0;
}