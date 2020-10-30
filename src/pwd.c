#include <unistd.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include "print.h"

#define SIZE 20000

/* Create a char * that contains the PWD */
char * getPWD (){
    char * path = (char *) malloc (sizeof(char) * SIZE);
    strcpy(path,getenv("PWD"));
    return path;
}

/* Print the values of environment variable PWD */
void pwd (){
    char * path = getPWD();
    strcat(path,"\n\0");
    print(path);
    free(path);
}
int isAbsolute (char * path){
    if (path[0] == '/')
        return 0;
    return -1;
}

char * duplicate (char * str){
    char * res = malloc (sizeof(char) * strlen(str));
    strcpy(res,str);
    return res;
}

/* Tell if a file is a tar, auxiliary function */
int isTar (char * name){
    int length = strlen(name);
    if (length <= 4)
        return 0;

    //Get the extention of the name
    char * extention = & name [length-4];
    if (strcmp(extention,".tar") == 0)
        return 1;
    return 0;
}


/* Tell if the path is in a tar */
int isInTar (char * path){
    char * pathCopy = duplicate(path);
    char * decompose = strtok(pathCopy, "/");
    while (decompose != NULL){
        if (isTar(decompose) == 1)
            return 1;
        decompose = strtok (NULL,"/");
    }
    return 0;
}

/* Divide the path in two, 
 * the first is the path to the tar file, 
 * the seconde is the rest (without the / just after ".tar")*/
char ** dividePathWithTar (char * path){
    char ** res = (char ** ) malloc (sizeof(char *) * 2);
    char * pathcpy = duplicate(path);
    char * separateur = strstr (pathcpy,".tar");
    if (separateur == NULL){
        print("Erreur Path without tar !!!");
        return NULL;
    }
    * (separateur + 4)  = '\0';
    res[0] = pathcpy;
    res[1] = separateur + 5;
    return res;
}

char * relatifToAbsolute (char * relatifPath){
    char * path = getPWD();
    strcat(path,"/");
    strcat(path,relatifPath);
    return path;
}

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

/* Delete . and .. from absolute path
Warning, if there where a / a the end of absolute, it will be deleted!
Might have to concatane a "/" at the end */
char * pathWithoutPoint (char * absolute){
    char * resultat = malloc (sizeof(char)* (strlen(absolute)+1));
    char * res [numberOfSlash(absolute)];
    int pointeur;

    removePointFromPath(res,&pointeur,absolute);

    for (int i = 0; i< pointeur ; i ++){
        strcat(resultat,"/");
        strcat(resultat,res[i]);
    }

    return resultat;
}


char * pathTreated (char * path){
    char * res = (isAbsolute(path) == 0)? path : relatifToAbsolute(path);
    return pathWithoutPoint(res);
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

