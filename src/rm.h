#ifndef RM_H
#define RM_H

int rmInTar(char * archive, char * path);
void rmR_aux (char * path);
void rmR (char ** argv);
void rm_aux(char * path);
void rm(char ** argv);

#endif