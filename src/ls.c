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
    char strpermission [10];
    memset(strpermission,'\0',10);
    char * permission [] = {"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};
    int userPerm = (header -> mode)[4] - '0';
    int groupPerm= (header -> mode)[5] - '0';
    int otherPerm = (header -> mode)[6] - '0';
    sprintf(strpermission,"%s%s%s",permission[userPerm],permission[groupPerm],permission[otherPerm]);

    /********** Calculate the user name (if not found, just place the id) *******/
    char struser [50];
    memset(struser,'\0',50);
    if(getpwuid(atoi(header -> uid)) != NULL)
        strcat(struser, getpwuid(atoi(header -> uid)) -> pw_name);
    else
        strcat(struser,header -> uid);

    /********** Calculate the group name (if not found, just place the id) *******/
    char strgrp [50];
    memset(strgrp,'\0',50);
    if(getgrgid(atoi(header -> gid)) != NULL)
        strcat(strgrp,getgrgid(atoi(header -> gid)) -> gr_name );
    else
        strcat(strgrp,header -> gid);

    /********** Calculate the string representation for the modified time *******/
    char time [20];
    time_t modifiedtime = atoi(header -> mtime);
    strftime(time, sizeof(time), " %b. %d %H:%M ", localtime(&modifiedtime));

    char format [1000];
    sprintf(format, "%c%s %s %s %s %s %s\n",regular,strpermission,struser,strgrp,header -> size,time, header -> name + repertoryLength);
    print(format);
}

/* FIXME : pour l'instant on va le faire que sur un argument, il faudra modifier pour plusieur argument */
void lsLong (char * path) {
    
    char ** division = dividePathWithTar (path);
    int fdArchive = openArchive(division[0],O_RDONLY);
    struct posix_header * buf = malloc (BLOCKSIZE);
        
    char repertoire [sizeof(char) * (strlen(division[1]) + 1)];
    strcpy (repertoire,division[1]);

    if (strlen(division[1]) > 0)
        strcat (repertoire,"/");

    replaceCurseurToStart (fdArchive);
    while(getHeader(fdArchive,buf) == 0 ){
        if (isInRepertory(repertoire,buf -> name) == 0){
            printLongFormat (strlen(repertoire),buf);
        }
    }
    
    print("\n");
}


/* FIXME : pour l'instant on va le faire que sur un argument, il faudra modifier pour plusieur argument */
void ls (char * path) {
    
    char ** division = dividePathWithTar (path);
    int fdArchive = openArchive(division[0],O_RDONLY);
    struct posix_header * buf = malloc (BLOCKSIZE);
        
    char repertoire [sizeof(char) * (strlen(division[1]) + 1)];
    strcpy (repertoire,division[1]);

    if (strlen(division[1]) > 0)
        strcat (repertoire,"/");

    replaceCurseurToStart (fdArchive);
    while(getHeader(fdArchive,buf) == 0 ){
        if (isInRepertory(repertoire,buf -> name) == 0){
            print(buf -> name + strlen(repertoire));
            print("   ");

        }
    }
    
    print("\n");
}
