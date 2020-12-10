#ifndef MKDIR_H
#define MKDIR_H

int mkdirInTar(char * archive, char * path);
void myMkdir_aux (char * path);
void myMkdir (char ** argv);

#endif