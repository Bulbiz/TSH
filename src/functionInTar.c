#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "tar.h"
#include "pathTreatement.h"

#define BLOCKSIZE 512

void replaceCurseurToStart (int fd){
    lseek(fd,0,SEEK_SET);
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
    replaceCurseurToStart (fd);
    struct posix_header * h = malloc(BLOCKSIZE);
    int tmp = getHeader(fd, h);
    while(tmp == 0) {
        tmp = getHeader(fd, h);
    }

    if(tmp == -2) {
        lseek(fd, -BLOCKSIZE, SEEK_CUR);
    }
}

int getFileSizeFromHeader (struct posix_header * buf){
    int numberBlock = 0;
    sscanf(buf -> size ,"%o", &numberBlock);
    numberBlock = (numberBlock + 512 -1) /512;
    return numberBlock * BLOCKSIZE;
}

/*  Return 0 if the file is found, else -1 */
int searchFile (int fd,struct posix_header * buf, char * name){
    replaceCurseurToStart (fd);
    while (getHeader(fd,buf) == 0){
        if(strcmp(buf->name, name) == 0){
            lseek(fd,-getFileSizeFromHeader(buf),SEEK_CUR);
            return 0;
        }
    }
    return -1;
}

/*  Return size if the file is found, else -1 */
int searchFileSize (int fd,struct posix_header * buf, char * name){
    size_t size = 0;
    replaceCurseurToStart (fd);
    while (getHeader(fd,buf) == 0){
        if(strcmp(buf->name, name) == 0){
            int numberBlock = 0;
            sscanf(buf -> size ,"%o", &numberBlock);
            numberBlock = (numberBlock + 512 -1) /512;
            size = numberBlock * BLOCKSIZE + (BLOCKSIZE); 
            return size;
        }
    }
    return -1;
}

int isARepertoryInTar (char * destination){
    char ** division = dividePathWithTar(duplicate(destination));
    int fd = openArchive(division[0], O_RDONLY);
    struct posix_header * buf = malloc (BLOCKSIZE);
    searchFile (fd, buf, division[1]);
    if(buf -> typeflag == '5'){
        free(buf);
        close(fd);
        return 0;
    }else{
        free(buf);
        close(fd);
        return -1;
    }
}

int isARepertoryOutsideTar (char * destination){
    struct stat statbuf;
    stat(destination, &statbuf);
    if (statbuf.st_mode == '5'){
        print("CCCCCCCCCCCCCCCCCCCC\n");
        return 0;
    }else{
        print("DDDDDDDDDDDDDDDDD\n");
        return -1;
    }
}

int isARepertory (char * destination){
    if(isInTar(destination) == 0){
        print("AAAAAAAAAAAAA\n");
        return isARepertoryInTar(destination);
    }else{
        print("BBBBBBBBBBBBB\n");
        return isARepertoryOutsideTar (destination);
    }
    
}

/* Fonction de test ! n'intervient pas dans le projet !*/
void printTar (int fd){
    printf("Lancement du print\n");
    replaceCurseurToStart(fd);
    struct posix_header * h = malloc (BLOCKSIZE);
    while (getHeader(fd,h) == 0){
        printf("File : %s; Size : %s; FileType : %c;GUD : %s; UID : %s\n",h -> name,h->size, h->typeflag,h -> gid, h -> uid);
    }
    replaceCurseurToStart(fd);
    printf("Fin du print\n");
}

/**** Fonction récupérer depuis le TP1 de système ****/

void set_checksum(struct posix_header *hd) {
  memset(hd->chksum,' ',8);
  unsigned int sum = 0;
  char *p = (char *)hd;
  for (int i=0; i < BLOCKSIZE; i++) { sum += p[i]; }
  sprintf(hd->chksum,"%06o",sum);
}

/* Check that the checksum of a header is correct */

int check_checksum(struct posix_header *hd) {
  unsigned int checksum;
  sscanf(hd->chksum,"%o ", &checksum);
  unsigned int sum = 0;
  char *p = (char *)hd;
  for (int i=0; i < BLOCKSIZE; i++) { sum += p[i]; }
  for (int i=0;i<8;i++) { sum += ' ' - hd->chksum[i]; }
  return (checksum == sum);
}
/* ******************************************************/

struct posix_header * createBloc0 (){
    struct posix_header * h = malloc (BLOCKSIZE);
    memset(h,0,BLOCKSIZE);
    return h;
}

char fileType (mode_t mode){
    switch (mode & S_IFMT) /* S_IFMT is the mask to have the filetype */
    {
        case S_IFBLK : return BLKTYPE;break; /* block special */
        case S_IFCHR : return CHRTYPE;break; /* character special */
        case S_IFIFO : return FIFOTYPE;break; /* FIFO special */
        case S_IFDIR : return DIRTYPE;break; /* directory */
        case S_IFLNK : return LNKTYPE;break; /* link */
        default : return REGTYPE; /* regular file */
    }
}

char * convertModeToChar (mode_t mode){
    char * buf = malloc (sizeof(char) * 12);
    sprintf(buf,"%011o",(~S_IFMT & mode));
    return buf + 4;
}

struct posix_header * createHeader (char * name, struct stat information){
    struct posix_header *  h = createBloc0();
    
    strcpy(h -> name, name);
    sprintf(h -> mode,"%s",convertModeToChar(information.st_mode));
    sprintf(h -> uid,"%d", information.st_uid);
    sprintf(h -> gid,"%d", information.st_gid);
    sprintf(h -> size,"%011lo",information.st_size);
    sprintf(h -> mtime,"%ld",time(NULL));
    h -> typeflag = fileType(information.st_mode);
    sprintf(h -> magic,TMAGIC);
    sprintf(h -> version,TVERSION);
    set_checksum(h);

    if(!check_checksum(h))
        perror("Checksum :");

    return h;
}

struct posix_header * createHeaderFolder (char * name){
    struct posix_header *  h = createBloc0();
    
    strcpy(h -> name, name);
    sprintf(h -> mode,"%s","0000700");
    long unsigned int sizeFolder = 0;
    sprintf(h -> size,"%011lo", sizeFolder);
    sprintf(h -> mtime,"%ld",time(NULL));
    h -> typeflag = '5';
    sprintf(h -> magic,TMAGIC);
    sprintf(h -> version,TVERSION);
    set_checksum(h);

    if(!check_checksum(h))
        perror("Checksum :");

    return h;
}

struct posix_header * createHeaderFromFile (int fd, char * newName){  
    struct stat information;
    fstat(fd,&information);
    return createHeader(newName,information);
}

char * getFileContentForTar (int fd, int * size){
    replaceCurseurToStart (fd);
    struct stat buf;
    fstat(fd,&buf);
    *size = ((buf.st_size + BLOCKSIZE -1) /BLOCKSIZE)* BLOCKSIZE;
    char * content = malloc (*size);
    memset(content,0,*size);

    if (read(fd,content,buf.st_size) == -1)
        perror("GetFileContent: ");
    return content;
}

void addFileToTar (int archive, struct posix_header * headerfile, char * contentfile, int size){
    passArchive(archive);
    if(write(archive,headerfile,BLOCKSIZE) < 0 || write(archive,contentfile,size) < 0)
        perror("addFileToTar");
}

/*
Warning : archive have to be openned with O_RDWR or it will not succed!
FIXME: Verify that the name isn't already here and if the path is valid !
*/
void copyFileToTar (int archive, int file,char * nametar){
    int size;
    struct posix_header * headerfile = createHeaderFromFile(file,nametar);
    char * contentfile = getFileContentForTar(file,&size);

    addFileToTar(archive,headerfile,contentfile,size);

    free(headerfile);
    free(contentfile);
}

//à tester
//return the size of the archive from the file path to the end
size_t getSizeAfterFile (char * path, int fd){
    replaceCurseurToStart(fd);
    struct posix_header * buffer = malloc (BLOCKSIZE);
    size_t size = 0;
    if(searchFile (fd, buffer, path) == -1){   //place the cursor on the FilePath
        free(buffer);
        return -1;
    }
    
    passContent (fd, buffer);

    while(getHeader(fd, buffer) == 0){        //start counting
        int numberBlock = 0;
        sscanf(buffer -> size ,"%o", &numberBlock);
        numberBlock = (numberBlock + 512 -1) /512;
        size += numberBlock * BLOCKSIZE + (BLOCKSIZE);  //size content + posix header
    }
    free(buffer);
    return size;
}

//à tester
char * getContentUntilPathFile(char * path, int fd, size_t size){
    replaceCurseurToStart (fd);
    if (size == -1)
        perror("Fichier introuvable");

    char * res = malloc (sizeof(char) * (size));
    struct posix_header * buffer = malloc (BLOCKSIZE);
    searchFile (fd, buffer, path);   //place the cursor on the FilePath
    passContent (fd, buffer);

    if(read (fd,res,size) < 0)
        perror("read");
    free(buffer);
    return res;
}

//if in repertory, 0 else -1
int isInRepertory (char * repertory, char * filename){
    return (
        strlen (repertory) < strlen(filename) && 
        strncmp(repertory,filename,strlen(repertory)) == 0 && 

        (numberOfSlash (filename) == numberOfSlash(repertory)  || 
        (numberOfSlash (filename) == numberOfSlash(repertory) + 1 && filename[strlen(filename) - 1] == '/'))) ? 0 : -1;
}
//return 0 if a file is in the repertory in the archive, else, return -1
int fileInRepertory(int fd, char * repertory){
    struct posix_header * buf = malloc(BLOCKSIZE);
    while(getHeader(fd,buf) == 0 ){
        if (isInRepertory(repertory, buf -> name) == 0){
            free(buf);
            return 0; 
        }
    }
    free(buf);
    return -1;
}


int getArgc (char ** argv){
    int i = 0;
    while (argv[i] != NULL){
        i++;
    }
    return i;
}

void executeCommandExterne (char ** argv){
    int child = fork ();

    switch (child){
        case -1 : perror ("Command execution"); break;
        case 0 ://child 
            if( execvp (argv[0],argv) == -1){
                perror ("Execution failure");
                exit(0);
            }   
            break;
        default ://father
            wait(NULL);
        break;
    }
}