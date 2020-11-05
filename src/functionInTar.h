#ifndef FUNCTIONINTAR_H
#define FUNCTIONINTAR_H
void print (char * message);
int openArchive (char * pathname, int flags);
int readHeader (int fd, struct posix_header * buffer);
char * getContent (int fd, struct posix_header * header);
void passContent (int fd, struct posix_header * header);
int getHeader(int fd, struct posix_header *header);
void passArchive(int fd);
int searchFile (int fd,struct posix_header * buf, char * name);

/*Function from TP1 Systeme*/
int check_checksum(struct posix_header *hd);
void set_checksum(struct posix_header *hd);


char * getPWD ();
void pwd ();
int isTar (char * name);
int isInTar (char * path);
char ** dividePathWithTar (char * path);
char * relatifToAbsolute (char * relatifPath);
int isAbsolute (char * path);
char * pathWithoutPoint (char * absolute);
char * duplicate (char * str);
char * pathTreated (char * path);


#endif
