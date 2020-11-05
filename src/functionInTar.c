#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "tar.h"

#define BLOCKSIZE 512

void print (char * message){
    if (write (STDIN_FILENO,message,strlen(message)) == -1)
        perror("Print :");
}

int openArchive (char * pathname, int flags){
    int tmp = open(pathname, flags);
    if(tmp < 0)
        perror("open");
    return tmp;
}

int readHeader (int fd, struct posix_header * buffer){
    int tmp = read(fd, buffer, BLOCKSIZE);
    if(tmp < 0)
        perror("read");
    return tmp;
}

char * getContent (int fd, struct posix_header * header){
    int numberBlock = 0;
    sscanf(header -> size ,"%o", &numberBlock);
    numberBlock = (numberBlock + BLOCKSIZE -1) /BLOCKSIZE;
    char * message =(char *) malloc (sizeof(char) * BLOCKSIZE * numberBlock);
    read (fd, message, sizeof(char) * BLOCKSIZE * numberBlock);
    return message;
}

void passContent (int fd, struct posix_header * header){
    int numberBlock = 0;
    sscanf(header -> size ,"%o", &numberBlock);
    numberBlock = (numberBlock + BLOCKSIZE -1) /BLOCKSIZE;
    lseek(fd, BLOCKSIZE * numberBlock, SEEK_CUR);
}

int getHeader(int fd, struct posix_header * header) {
    int tmp = readHeader(fd, header);

    if(tmp == BLOCKSIZE) {
        if(*header->name == '\0') {
            return -2;
        }
        passContent(fd, header);
        return 0;
    }
    return -1;
}

void passArchive(int fd) {
    struct posix_header * h = malloc(BLOCKSIZE);
    int tmp = getHeader(fd, h);
    while(tmp == 0) {
        tmp = getHeader(fd, h);
    }

    if(tmp == -2) {
        lseek(fd, -BLOCKSIZE, SEEK_CUR);
    }
}

/*  Return 0 if the file is found, else -1 */
int searchFile (int fd,struct posix_header * buf, char * name){
    while (getHeader(fd,buf) == 0){
        if(strcmp(buf->name, name) == 0)
            return 0;
    }
    return -1;
}


char * getFileContentForTar (int fd, int * size){
    struct stat buf;
    fstat(fd,&buf);
    *size = ((buf.st_size + BLOCKSIZE -1) /BLOCKSIZE)* BLOCKSIZE;
    char * content = malloc (*size);
    memset(content,0,*size);
    if (read(fd,content,buf.st_size) == -1)
        perror("GetFileContent: ");
    return content;
}

#define SIZE 20000

/* Create a char * that contains the PWD */
char * getPWD (){
    char * path = (char *) malloc (sizeof(char) * SIZE);
    strcpy(path,getenv("PWD"));
    return path;
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

