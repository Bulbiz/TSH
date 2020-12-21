#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
int saveStdOut;

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

/*count the number of command separe by '|' */
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

/*replace ' ' by '\0' in the String command */
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
    char ** argv = malloc (sizeof(char *) * (nbOfArg + 5)); // There is a place to add one argument if wanted 
    for (int i = 0; i < nbOfArg + 5; i++)
        argv[i] = NULL;

    while(command[i]!= '\0'){
        if (command[i] == ' ')
            i += jumpSpace(command + i);
        else{
            argv[pointer++] = (command + i);
            i += jumpArgument(command + i);
        }
    }

    replaceSpace (command);

    return argv;
}

/*tranform all path in argv to path absolute, except the options and the commande*/
char ** transformPathOfArgv (char ** argv){
    for (int i = 1; i< getArgc(argv); i++){
        if(argv[i][0] != '-')
            argv[i] = pathTreated(argv[i]);
    }
    return argv;
}

/*if the argv has the option in the argument then return 0 else -1 */
int hasOption (char * option, char ** argv ){
    int i = 0;
    while(argv[i] != NULL){
        if(strcmp(argv[i], option) == 0)
            return 0;
        i++; 
    }
    return -1;
}

/* delete all "-" option in the argv */
char **  deleteOption (char ** argv){
    char ** newArgv = malloc (sizeof (char *) * (getArgc(argv) + 2));
    for (int i=0; i < getArgc(argv) + 2; i++)
        newArgv[i] = NULL;
    
    int count = 0;
    for (int i = 0; argv[i] != NULL; i ++){

        if (argv[i][0] != '-'){
            newArgv[count] = argv[i];
            count ++;
        }
    }
    return newArgv;
}

/*count the number of ">" and "<" */
int numberOfRedirection (char * userInput){
    int cmp = 0;
    for(int i=0; i<LIMIT; i++)
        if(buffer[i] == '>' || buffer[i] == '<')
            cmp++;
    return cmp;
}

/*cut the userInput in 2 string separate with '>', and put them in a (char **)
if the direction is 1 then we have ">" so the redirection[0] output will be write to redirection[1]
if the direction is 2 then we have "<" so the redirection[1] output will be write to redirection[0]*/
char ** inputCutterRedirection (char * userInput){
    char ** redirection = malloc (sizeof(char *) * 2);
    int direction = 0;
    for (int i = 0; i < strlen(userInput); i++){
        if(userInput[i] == '>'){
            userInput[i] = '\0';
            direction = 1;
        }
        if(userInput[i] == '<'){
            userInput[i] = '\0';
            direction = 2;
        }      
    }
    if(direction == 1){
        redirection[0] = userInput;
        redirection[1] = userInput + strlen(userInput) + 1;
        /*redirection[1] = redirection[1] + jumpSpace(redirection[1]);
        replaceSpace(redirection[1]);
        redirection[1] = pathTreated (redirection[1]);*/
    }
    else if (direction == 2){
        redirection[0] = userInput + strlen(userInput) + 1;
        /*redirection[0] = redirection[0] + jumpSpace(redirection[0]);
        replaceSpace(redirection[0]);
        redirection[0] = pathTreated (redirection[0]);*/
        redirection[1] = userInput;
    }
    print(redirection[0]);
    print("\n");
    print(redirection[1]);
    return redirection;
}

/*return 0 if there is 1 '>', -2 if there is 2 or more '>' 
and return -1 if there is no '>' */
int checkRedirection (char * userInput){
    if (numberOfRedirection (userInput) == 1)
        return 0;
    if (numberOfRedirection (userInput) > 1){
        return -2;
    }
    return -1;
}

void restoreStdOut (){
    dup2(saveStdOut, STDOUT_FILENO);
}

/*main function that executes the shell 
*/
void shell(){
    while(TRUE){
        userInput ();
        char ** argv;
        // aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
        char * fileRedirection;
        char * trash;
        int fdRedirection = -1;
        int checkRedi = checkRedirection(buffer);
        if (checkRedi == -2){
            print("trop d'argument pour les redirections");
            continue;
        }

        if(checkRedi == 0){
            char ** tmp = inputCutterRedirection(buffer);
            argv = getArgument(tmp[0]);
            fileRedirection = pathTreated (tmp[1] + jumpSpace(tmp[1]));
            replaceSpace (fileRedirection);
            if(isInTar(fileRedirection) == 0){
                char ** pathTmp = dividePathWithTar (fileRedirection);
                trash = malloc (sizeof (char) * (strlen(pathTmp[0]) + 10));
                memset (trash, '\0' , strlen(pathTmp[0]) + 10);
                sprintf(trash, "%s%s", getRepertoryRepertory (pathTmp[0]), "trash");
                fdRedirection = open (trash, O_WRONLY | O_CREAT , S_IRUSR | S_IWUSR);  
            }else{
                fdRedirection = open (fileRedirection, O_WRONLY | O_CREAT , S_IRUSR | S_IWUSR);       
            }
            dup2 (fdRedirection, STDOUT_FILENO);

        }else{
            argv = getArgument(buffer);
        }
        // aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
        if (getArgc(argv) == 0)
            continue;

        if(strcmp (argv[0],"cd") == 0){
            argv = transformPathOfArgv(argv);
            cd(argv);

        }else if(strcmp (argv[0],"pwd") == 0){
            argv = transformPathOfArgv(argv);
            pwd();

        }else if(strcmp (argv[0],"mkdir") == 0){
            argv = transformPathOfArgv(argv);
            myMkdir (argv);

        }else if(strcmp (argv[0],"rmdir") == 0){
            argv = transformPathOfArgv(argv);
            myRmdir (argv);

        }else if(strcmp (argv[0],"mv") == 0){
            argv = transformPathOfArgv(argv);
            mv(argv);

        }else if(strcmp (argv[0],"cp") == 0){
            argv = transformPathOfArgv(argv);
            if (hasOption ("-r", argv) == 0)
                cpR(deleteOption(argv));
            else
                cp(argv);

        }else if(strcmp (argv[0],"rm") == 0){
            argv = transformPathOfArgv(argv);
            if (hasOption ("-r", argv) == 0)
                rmR(deleteOption(argv));           
            else
                rm (argv);

        }else if(strcmp (argv[0],"ls") == 0){
            argv = transformPathOfArgv(argv);
            if (hasOption ("-l", argv) == 0)
                lsL(deleteOption(argv));
            else
                ls(argv);

        }else if(strcmp (argv[0],"cat") == 0){
            argv = transformPathOfArgv(argv);
            cat (argv);

        }else if(strcmp (argv[0],"exit") == 0){

            exit(0);

        }else{

            executeCommandExterne(argv);
            
        }
        //aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
        if(checkRedi == 0){
            if(isInTar(fileRedirection) == 0){
                char ** pathTmp = dividePathWithTar (fileRedirection);
                struct posix_header * h = malloc(BLOCKSIZE);
                int fdArchive = openArchive(pathTmp[0], O_RDWR);

                if(fdArchive < 0){
                    unlink(trash);
                    restoreStdOut();
                    continue;
                }

                if(searchFile(fdArchive, h, pathTmp[1]) == 0){
                    close(fdArchive);
                    rmInTar(pathTmp[0], pathTmp[1]);
                }

                cpOutsideTarInTar(pathTmp[0], trash, pathTmp[1]);
                unlink(trash);              
            }
            restoreStdOut();   
        }
        //aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
    }
}




/*execute the shell*/
int main (){
    saveStdOut = dup(STDOUT_FILENO);
    cwd = malloc (SIZE);
    getcwd(cwd,SIZE);
    shell();
    return 0;
}
