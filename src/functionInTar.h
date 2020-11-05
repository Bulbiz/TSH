#ifndef FUNCTIONINTAR_H
#define FUNCTIONINTAR_H

int openArchive (char * pathname, int flags);
int readHeader (int fd, struct posix_header * buffer);
char * getContent (int fd, struct posix_header * header);
void passContent (int fd, struct posix_header * header);
int getHeader(int fd, struct posix_header *header);
void passArchive(int fd);
int searchFile (int fd,struct posix_header * buf, char * name);
struct posix_header createBloc0();
struct posix_header createHeaderFromFile (int fd, char * newName);

#endif
