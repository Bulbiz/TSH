#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define SIZE 20000

/* current working directory */
char * cwd;

/*print the char * message in the shell*/
void print (char * message){
    if (write (STDOUT_FILENO,message,strlen(message)) == -1)
        perror("Print");
}

/* Create a char * that contains the PWD */
char * getPWD (){
    char * path = (char *) malloc (sizeof(char) * SIZE);
    strcpy(path,cwd);
    return path;
}

/*return 0 if the path is Absolute, else -1*/
int isAbsolute (char * path){
    if (path[0] == '/')
        return 0;
    return -1;
}

/*return a copy of str (String), give some extra space just in case we need it */
char * duplicate (char * str){
    char * res = malloc (sizeof(char) * (strlen(str) + 10));
    memset(res,'\0',strlen(str) + 10);
    strcpy(res,str);
    return res;
}

/* return 0 if the path is in a tar else -1*/
int isInTar (char * path){
    return (strstr(path,".tar") != NULL) ? 0 : -1;
}

/* Divide the path in two, 
 * the first is the path to the tar file, 
 * the seconde is the rest (without the / just after ".tar")*/
char ** dividePathWithTar (char * path){
    char ** res = (char ** ) malloc (sizeof(char *) * 2);
    char * pathcpy = duplicate(path);
    char * separateur = strstr (pathcpy,".tar");

    if (separateur == NULL){
        print("Erreur Path without tar !!!\n");
        return NULL;
    }
    * (separateur + 4)  = '\0';
    res[0] = pathcpy;
    res[1] = (strlen(path) == strlen(res[0])) ? separateur + 4 : separateur + 5;
    return res;
}

/*convert the relatifPath to absolutePath*/
char * relatifToAbsolute (char * relatifPath){
    char * path = getPWD();
    strcat(path,"/");
    strcat(path,relatifPath);
    return path;
}

/*count the number of slash in the path*/
int numberOfSlash(char * path){
    int cmp = 0;
    for(char * c = path; *c != '\0'; c ++){
        if (*c == '/')
            cmp ++;
    }
    return cmp;
}

/*Auxilary function :Return res completed without . or .. */
void removePointFromPath (char ** res, int * pointeur, char * absolute){
    char * decompose = strtok(absolute, "/");
    (* pointeur) = 0;
    while (decompose != NULL){
        if(strcmp(decompose,".") == 0){
            decompose = strtok(NULL, "/");
            continue;
        }else if(strcmp(decompose,"..") == 0){
            decompose = strtok(NULL, "/");
            (*pointeur) --;
            continue;
        } else {
            res[(*pointeur) ++] = decompose;
            decompose = strtok (NULL,"/");
        }
    }
}

/* Delete . and .. from absolute path */
char * pathWithoutPoint (char * absolute){
    char * end = absolute[strlen(absolute) - 1] == '/' ? "/" : "";
    char * resultat = malloc (sizeof(char)* (strlen(absolute) + 2));
    memset(resultat, '\0', strlen(absolute) + 2);
    char * res [numberOfSlash(absolute)];
    int pointeur;

    removePointFromPath(res,&pointeur,absolute);

    for (int i = 0; i< pointeur ; i ++){
        strcat(resultat,"/");
        strcat(resultat,res[i]);
    }
    strcat(resultat,end);
    return resultat;
}

/*convert every path to absolute path without . and ..*/
char * pathTreated (char * path){
    char * res = (isAbsolute(path) == 0)? path : relatifToAbsolute(path);
    return pathWithoutPoint(res);
}

/*Add a '/' at the end of the path if there wasn't one */
char * addSlash (char * path){
    char * duplication = duplicate(path);
    if(duplication[strlen(duplication) - 1] != '/')
        strcat(duplication,"/");
    return duplication;
}

/* Get the name of the repertory of a file.
For exemple, the file aaa/toto will return aaa/*/
char * getFileRepertory (char * path){
    path = duplicate(path);
    int i = strlen(path) -1;
    while(path[i] != '/')
        i--;
    path[i + 1] = '\0';
    return path;
}

/* Get the name of the repertory of a repertory.
For exemple, the file aaa/toto/ will return aaa/*/
char * getRepertoryRepertory (char * path){
    path = duplicate(path);
    int i = strlen(path) -2;
    while(path[i] != '/')
        i--;
    path[i + 1] = '\0';
    return path;
}