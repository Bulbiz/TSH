#ifndef PATHTREATMENT_H
#define PATHTREATMENT_H

#define SIZE 20000

char * cwd;

void print (char * message);
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