#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#include "tar.h"
#include "functionInTar.h"
#include "pathTreatement.h"

void printLongFormat (int repertoryLength,struct posix_header * header){
    char regular = (header -> typeflag == '5')? 'd' : '-';

    /******* Calculate the string representation for the permission ************/
    char * strpermission = malloc (sizeof(char) * 40);
    memset(strpermission,'\0',40);
    char * permission [] = {"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};
    int userPerm = (header -> mode)[3] - '0';
    int groupPerm= (header -> mode)[4] - '0';
    int otherPerm = (header -> mode)[5] - '0';
    sprintf(strpermission,"%s%s%s",permission[userPerm],permission[groupPerm],permission[otherPerm]);

    /********** Calculate the user name (if not found, just place the id) *******/
    char * struser = malloc (sizeof(char) * 100);
    memset(struser,'\0',100);
    if(getpwuid(atoi(header -> uid)) != NULL)
        strcat(struser, getpwuid(atoi(header -> uid)) -> pw_name);
    else
        strcat(struser,header -> uid);

    /********** Calculate the group name (if not found, just place the id) *******/
    char * strgrp = malloc(sizeof(char) * 100);
    memset(strgrp,'\0',100);
    if(getgrgid(atoi(header -> gid)) != NULL)
        strcat(strgrp,getgrgid(atoi(header -> gid)) -> gr_name );
    else
        strcat(strgrp,header -> gid);

    /********** Calculate the string representation for the modified time *******/
    char * time = malloc (sizeof(char) * 100);
    memset(time,'\0',100);
    time_t modifiedtime = atoi(header -> mtime);
    strftime(time, sizeof(time), " %b. %d %H:%M ", localtime(&modifiedtime));

    char * format = malloc (sizeof(char) * 1000);
    memset(format,'\0',1000);
    sprintf(format, "%c%s %s %s %s %s %s\n",regular,strpermission,struser,strgrp,header -> size,time, header -> name + repertoryLength);
    print(format);

    free(strpermission);
    free(struser);
    free(strgrp);
    free(time);
    free(format);
}

void lsLong (char * path) {
    path = addSlash (path);
    char ** division = dividePathWithTar (path);
    int fdArchive = openArchive(division[0],O_RDONLY);
    struct posix_header * buf = malloc (BLOCKSIZE);
        
    char repertoire [sizeof(char) * (strlen(division[1]) + 1)];
    strcpy (repertoire,division[1]);

    replaceCurseurToStart (fdArchive);
    while(getHeader(fdArchive,buf) == 0 ){
        if (isInRepertory(repertoire,buf -> name) == 0){
            printLongFormat (strlen(repertoire),buf);
        }
    }
    
    print("\n");
}

void lsL (char ** argv) {
    argv[1] = getArgc(argv) == 1 ? pathTreated(getPWD()) : argv[1];
    if (isInTar(argv[1]) == 0)
        lsLong(argv[1]);
    else{
        argv[getArgc(argv)] = "-l";
        executeCommandExterne(argv);
    }
}


/* FIXME : pour l'instant on va le faire que sur un argument, il faudra modifier pour plusieur argument */
void ls_aux (char * path) {
    path = addSlash (path);
    char ** division = dividePathWithTar (path);
    int fdArchive = openArchive(division[0],O_RDONLY);
    struct posix_header * buf = malloc (BLOCKSIZE);
        
    char repertoire [sizeof(char) * (strlen(division[1]) + 1)];
    strcpy (repertoire,division[1]);

    replaceCurseurToStart (fdArchive);
    while(getHeader(fdArchive,buf) == 0 ){
        if (isInRepertory(repertoire,buf -> name) == 0){
            print(buf -> name + strlen(repertoire));
            print("   ");

        }
    }
    
    print("\n");
}

void ls (char ** argv) {
    argv[1] = getArgc(argv) == 1 ? pathTreated(getPWD()) : argv[1];
    if (isInTar(argv[1]) == 0)
        ls_aux(argv[1]);
    else
        executeCommandExterne(argv);
}