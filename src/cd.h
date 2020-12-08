#ifndef CD_H
#define CD_H

int setPWD (char * path);
int cdInTar (char * path);
void cd_aux (char * path);
void cd (char ** argv);

#endif