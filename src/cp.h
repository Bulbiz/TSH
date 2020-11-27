#ifndef CP_H
#define CP_H

int cpTarInTar(char * archive, char * path, char * destination);
int cpTarInOutsideTar(char * archive, char * path, char * destination);
int cpOutsideTarInTar(char * archive, char * path, char * destination);
int cp1 (char * path, char * destination);
int cp2 (char * path, char * destination);
int cp3 (char * path, char * destination);
int cp(char ** argv);
#endif