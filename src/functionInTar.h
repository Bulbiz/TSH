#ifndef FUNCTIONINTAR_H
#define FUNCTIONINTAR_H

void print (char * message);
void printTar (int fd);

void replaceCurseurToStart (int fd);
int openArchive (char * pathname, int flags);
int readHeader (int fd, struct posix_header * buffer);
char * getContent (int fd, struct posix_header * header);
void passContent (int fd, struct posix_header * header);
int getHeader(int fd, struct posix_header *header);
void passArchive(int fd);

int searchFile (int fd,struct posix_header * buf, char * name);
int searchFileSize (int fd,struct posix_header * buf, char * name);
void copyFileToTar (int archive, int file,char * nametar);
void addFileToTar (int archive, struct posix_header * headerfile, char * contentfile, int size);
void copyFileToTar (int archive, int file,char * nametar);

struct posix_header createBloc0();
struct posix_header createHeaderFromFile (int fd, char * newName);

size_t getSizeAfterFile (char * path, int fd);
char * getContentUntilPathFile(char * path, int fd, size_t size);
int isInRepertory (char * repertory, char * filename);
int fileInRepertory(int fd, char * repertory);

#endif
