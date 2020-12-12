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

/* Replace the cursor at the start of the descriptor */
void replaceCurseurToStart (int fd){
    lseek(fd,0,SEEK_SET);
}

/* Open a archive and print a error if there is a failure */
int openArchive (char * pathname, int flags){
    int tmp = open(pathname, flags);
    if(tmp < 0)
        perror("open");
    return tmp;
}

/* Read 512 bites in the descripteur and initialise it in buffer, the cursor of the descriptor have to be just in front of the header */
int readHeader (int fd, struct posix_header * buffer){
    int tmp = read(fd, buffer, BLOCKSIZE);
    if(tmp < 0)
        perror("read");
    return tmp;
}

/* Read the content of the header in the descriptor, the cursor of the descriptor have to be just in front of the content */
char * getContent (int fd, struct posix_header * header){
    int numberBlock = 0;
    sscanf(header -> size ,"%o", &numberBlock);
    numberBlock = (numberBlock + BLOCKSIZE -1) /BLOCKSIZE;
    char * message =(char *) malloc (sizeof(char) * BLOCKSIZE * numberBlock);
    read (fd, message, sizeof(char) * BLOCKSIZE * numberBlock);
    return message;
}

/* Place the cursor of the descriptor after the content of the header */
void passContent (int fd, struct posix_header * header){
    int numberBlock = 0;
    sscanf(header -> size ,"%o", &numberBlock);
    numberBlock = (numberBlock + BLOCKSIZE -1) /BLOCKSIZE;
    lseek(fd, BLOCKSIZE * numberBlock, SEEK_CUR);
}

/* Read the header and pass the content in one go */
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

/* Place the cursor of the descriptor at the end of the archive */
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

/* Count the number of file/repertory in the archive */
int numberFileInArchive(int fd) {
    replaceCurseurToStart (fd);
    struct posix_header * h = malloc(BLOCKSIZE);
    int tmp = getHeader(fd, h);
    int count = 0;
    while(tmp == 0) {
        tmp = getHeader(fd, h);
        count ++;
    }

    if(tmp == -2) {
        lseek(fd, -BLOCKSIZE, SEEK_CUR);
    }
    return count;
}

/* Return the size of a header (after converting it with the Blocksize) */
int getFileSizeFromHeader (struct posix_header * buf){
    int numberBlock = 0;
    sscanf(buf -> size ,"%o", &numberBlock);
    numberBlock = (numberBlock + 512 -1) /512;
    return numberBlock * BLOCKSIZE;
}

/*  Search a file with the name in the descriptor, if found return 0 else -1 */
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

/*  Search a file with the name in the descriptor, if found return the size of the file else -1 */
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

/* Verify if destination is a repertory (destination have to be a path in a tarball) */
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

/* Verify if destination is a repertory (destination have to be a path outside of a tarball) */
int isARepertoryOutsideTar (char * destination){
    struct stat buffer;
    if (stat(destination,&buffer) == -1)
        return -1;

    if ((buffer.st_mode & S_IFMT) == S_IFDIR)
        return 0;
    else
        return -1;   
}

/* Verify if destination is a repertory */
int isARepertory (char * destination){
    if(isInTar(destination) == 0){
        return isARepertoryInTar(destination);
    }else{
        return isARepertoryOutsideTar (destination);
    }
    
}

/*  Test function, not directly used in the project ! 
    Print all the file in the archive with some information */
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

/* Initialise the chksum value for a pre constructed posix_header for the tar*/
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

/* Create a completely empty posix header */
struct posix_header * createBloc0 (){
    struct posix_header * h = malloc (BLOCKSIZE);
    memset(h,0,BLOCKSIZE);
    return h;
}

/* Extract the file type from a mode_t */
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

/* Convert a mode_t in a char [12] for the initialisation of a header */
char * convertModeToChar (mode_t mode){
    char * buf = malloc (sizeof(char) * 12);
    sprintf(buf,"%011o",(~S_IFMT & mode));
    return buf + 4;
}

/* Create a header from a stat */
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
        perror("Checksum");

    return h;
}

/* Create a header for a folder */
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
        perror("Checksum");

    return h;
}

/* Create a header from the descriptor of the file and initialise it with the new name */
struct posix_header * createHeaderFromFile (int fd, char * newName){  
    struct stat information;
    fstat(fd,&information);
    return createHeader(newName,information);
}

/* Return the content of fd (a descriptor for the file we want to get the content) */
char * getFileContentForTar (int fd, int * size){
    replaceCurseurToStart (fd);
    struct stat buf;
    fstat(fd,&buf);
    *size = ((buf.st_size + BLOCKSIZE -1) /BLOCKSIZE)* BLOCKSIZE;
    char * content = malloc (*size);
    memset(content,0,*size);

    if (read(fd,content,buf.st_size) == -1)
        perror("GetFileContent");
    return content;
}

/* Add to the archive the file with the header headerfile, the content contentfile and the size size */
void addFileToTar (int archive, struct posix_header * headerfile, char * contentfile, int size){
    passArchive(archive);
    if(write(archive,headerfile,BLOCKSIZE) < 0 || write(archive,contentfile,size) < 0)
        perror("addFileToTar");
}

/* Copy a file in the archive with the new name nametar
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
/* Return the size of the archive from the file path to the end */
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

/* Return the size bite of the archive */
char * getContentUntilPathFile(char * path, int fd, size_t size){
    replaceCurseurToStart (fd);
    if (size == -1)
        print("Fichier introuvable");

    char * res = malloc (sizeof(char) * (size));
    struct posix_header * buffer = malloc (BLOCKSIZE);
    searchFile (fd, buffer, path);   //place the cursor on the FilePath
    passContent (fd, buffer);

    if(read (fd,res,size) < 0)
        perror("getContentUntilPathFile");
    free(buffer);
    return res;
}

/* Verify if the file filename is in the repertory repertory */
int isInRepertory (char * repertory, char * filename){
    return (
        strlen (repertory) < strlen(filename) && 
        strncmp(repertory,filename,strlen(repertory)) == 0 && 
        (numberOfSlash (filename) == numberOfSlash(repertory)  || 
        (numberOfSlash (filename) == numberOfSlash(repertory) + 1 && filename[strlen(filename) - 1] == '/'))) ? 0 : -1;
}

/* Verify if the repertory have at least one file inside */
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

/* Return the number of argument */
int getArgc (char ** argv){
    int i = 0;
    while (argv[i] != NULL){
        i++;
    }
    return i;
}

/* Execute the command defined in the argv */
void executeCommandExterne (char ** argv){
    int child = fork ();

    switch (child){
        case -1 : perror ("executeCommandExterne"); break;
        case 0 ://child 
            if( execvp (argv[0],argv) == -1){
                perror ("executeCommandExterne");
                exit(0);
            }   
            break;
        default ://father
            wait(NULL);
        break;
    }
}