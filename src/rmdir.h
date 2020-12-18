#ifndef RMDIR_H
#define RMDIR_H

int fileInFolderInTar (char * archive, char * path);
int rmdirInTar(char * archive, char * path);
int myRmdir_aux (char * path);
int myRmdir (char ** argv);

#endif