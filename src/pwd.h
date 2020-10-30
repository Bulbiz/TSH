#ifndef PWD_H
#define PWD_H

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
