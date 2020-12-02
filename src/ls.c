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

void printLongFormat (int repertoryLegth,struct posix_header * header){
    char format [1000];
    memset(format,'\0',100);
    format[0] = (header -> typeflag == '5')? 'd' : '-';

    char * permission [] = {"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};
    int userPermission = (header -> mode)[4] - '0';
    int groupPermission = (header -> mode)[5] - '0';
    int otherPermission = (header -> mode)[6] - '0';
    strcat(format,permission[userPermission]);
    strcat(format,permission[groupPermission]);
    strcat(format,permission[otherPermission]);

    strcat(format," ");
    if(getpwuid(atoi(header -> uid)) != NULL)
        strcat(format, getpwuid(atoi(header -> uid)) -> pw_name);
    else
        strcat(format,header -> uid);
    
    strcat(format," ");
    if(getgrgid(atoi(header -> gid)) != NULL)
        strcat(format,getgrgid(atoi(header -> gid)) -> gr_name );
    else
        strcat(format,header -> gid);

    strcat(format," ");
    strcat(format,header -> size);

    char time [20];
    time_t modifiedtime = atoi(header -> mtime);
    strftime(time, sizeof(time), " %b. %d %H:%M ", localtime(&modifiedtime));
    strcat(format,time);

    strcat(format,header -> name + repertoryLegth);
    printf("-l : %s\n",format);
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
            //printLongFormat (strlen(repertoire),buf);
            print("   ");

        }
    }
    
    print("\n");
}
