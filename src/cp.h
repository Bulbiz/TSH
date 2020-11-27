#ifndef CP_H
#define CP_H

int cpTarInTar(char * archive, char * path, char * destination);
int cpTarInOutsideTar(char * archive, char * path, char * destination);
int cpOutsideTarInTar(char * archive, char * path, char * destination);
void cp1 (char * path, char * destination);
void cp2 (char * path, char * destination);
void cp3 (char * path, char * destination);

#endif