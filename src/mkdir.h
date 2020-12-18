#ifndef MKDIR_H
#define MKDIR_H

int mkdirInTar(char * archive, char * path);
int myMkdir_aux (char * path);
int myMkdir (char ** argv);

#endif