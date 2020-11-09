#ifndef FUNCTIONINTAR_H
#define FUNCTIONINTAR_H

int openArchive (char * pathname, int flags);
int readHeader (int fd, struct posix_header * buffer);
char * getContent (int fd, struct posix_header * header);
void passContent (int fd, struct posix_header * header);
int getHeader(int fd, struct posix_header *header);
void passArchive(int fd);

int searchFile (int fd,struct posix_header * buf, char * name);
void addFileToTar (int archive, struct posix_header * headerfile, char * contentfile, int size);
void copyFileToTar (int archive, int file,char * nametar);

struct posix_header createBloc0();
struct posix_header createHeaderFromFile (int fd, char * newName);

size_t getSizeAfterFile (char * path, int fd);
char * getContentUntilPathFile(char * path, int fd, size_t *size);
#endif
