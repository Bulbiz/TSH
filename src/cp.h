#ifndef CP_H
#define CP_H

int fileName (char * pathName);
int cpTarInTar(char * archive, char * path, char * destination);
int cpTarInOutsideTar(char * archive, char * path, char * destination);
int cpOutsideTarInTar(char * archive, char * path, char * destination);
int cp1 (char * path, char * destination);
int cp2 (char * path, char * destination);
int cp3 (char * path, char * destination);
int cp(char ** argv);

char ** nameOfAllFileInDirectory (int fd, char * path, int archiveSize);
int cpTarInTarOptionR(char * archive, char * path, char * destination);
int cpTarInOutsideTarOptionR(char * archive, char * path, char * destination);

#endif