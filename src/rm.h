#ifndef RM_H
#define RM_H

int rmInTar(char * archive, char * path);
void rm_aux(char * path);
void rm(char ** argv);

#endif