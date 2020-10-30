#ifndef PRINT_H
#define PRINT_H

char * getPWD ();
void pwd ();
int isTar (char * name);
int isInTar (char * path);
char ** dividePathWithTar (char * path);
char * relatifToAbsolute (char * relatifPath);
int isAbsolute (char * path);

#endif