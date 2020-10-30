#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "print.h"

#define TRUE 1
#define LIMIT 20000

//main function that executes the shell
void shell(){
    char * buffer = (char *) malloc(sizeof(char)*LIMIT);
    while(TRUE){
        read(STDIN_FILENO, buffer, LIMIT);  //we need a function who separate the option
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

        }else{
            print("commande inconnue");
        }
    }
}





/*int main (void){
    shell();
    return 0;
}*/