#ifndef FUNCTIONINTAR_H
#define FUNCTIONINTAR_H

int openArchive (char * pathname, int flags);
int readHeader (int fd, struct posix_header buffer);
char * getContent (struct posix_header header, int fd);
void passContent (struct posix_header header, int fd);

#endif