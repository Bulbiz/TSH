#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "tar.h"
#include "print.h"

#define BLOCKSIZE 512

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

struct posix_header createBloc0 (){
    struct posix_header * h = malloc (BLOCKSIZE);
    memset(h,0,BLOCKSIZE);
    return *h;
}

struct posix_header createHeader (char * name, char * mode,char * size,char typeflag){
    struct posix_header h = createBloc0();
    
    sprintf(h.name,"%s",name);
    sprintf(h.mode,"%s",mode);
    sprintf(h.size,"%s",size);
    h.typeflag = typeflag;
    sprintf(h.magic,TMAGIC);
    sprintf(h.version,TVERSION);
    set_checksum(&h);

    if(!check_checksum(&h))
        perror("checksum :");

    return h;
}

struct posix_header createHeaderDefault (char * name,char * size,char typeflag){
    return createHeader(name,"0000700",size,typeflag);
}

char fileType (mode_t mode){
    switch (mode & S_IFMT)
    {
        case S_IFBLK : return BLKTYPE;break; /* block special */
        case S_IFCHR : return CHRTYPE;break; /* character special */
        case S_IFIFO : return FIFOTYPE;break; /* FIFO special */
        case S_IFDIR : return DIRTYPE;break; /* directory */
        case S_IFLNK : return LNKTYPE;break; /* link */
        default : return REGTYPE; /* regular file */
    }
}

void sizeFormat (char * buf,off_t size){
    sprintf(buf,"%ld",size);
}

struct posix_header createHeaderFromFile (int fd, char * newName){  
    struct stat buf;
    fstat(fd,&buf);
    char size [12];
    sizeFormat(size,buf.st_size);
    return createHeaderDefault(newName,size,fileType(buf.st_mode));
}

int main (){
    /*int fd = openArchive("archive.tar",O_RDONLY);
    struct posix_header h;
    searchFile(fd,&h,"rep/");
    printf("Mode: %s\n",h.mode);*/
    int fd = open("toto",O_RDONLY);
    struct posix_header h = createHeaderFromFile(fd,"aaaAAAAAAAAA");
    printf("Name : %s\n\nSize : %s",h.name,h.size);
}