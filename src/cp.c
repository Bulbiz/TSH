#include "pathTreatement.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "tar.h"
#include "pwd.h"
#include "functionInTar.h"

//give the number for the name after the last '/'
int fileName (char * pathName){
    int i = strlen(pathName) -2;
    while(pathName[i] != '/')
        i--;
    return i;
}

int cpTarInTar(char * archive, char * path, char * destination){

    int fd = openArchive (archive, O_RDWR);
    struct posix_header * copyHeader = malloc (BLOCKSIZE);
    if(searchFile (fd, copyHeader, path) == -1){
        perror("fichier inexistant");
        return -1;
    }
  
    memset(copyHeader -> name, 0, 100);
    memcpy(copyHeader -> name, destination, (strlen(destination)));
    set_checksum(copyHeader);
    if(!check_checksum(copyHeader)){
        perror("Checksum :");
        return -1;
    }

    int filesize;
    sscanf(copyHeader -> size, "%o" , &filesize);
    char * copyContent = malloc (sizeof(char) * filesize);
    read (fd, copyContent, filesize);
    passArchive(fd);

    write(fd, copyHeader, BLOCKSIZE);
    write(fd, copyContent, filesize);

    free(copyHeader);
    free(copyContent);
    close(fd);
    return 0;
}

int cpTarInOutsideTar(char * archive, char * path, char * destination){

    int fd = openArchive (archive, O_RDONLY);

    struct posix_header * copyHeader = malloc (BLOCKSIZE);
    if(searchFile (fd, copyHeader, path) == -1){
        perror("fichier inexistant");
        return -1;
    }

    int filesize;
    sscanf(copyHeader -> size, "%o" , &filesize);

    char * copyContent = malloc (sizeof(char) * filesize);

    read (fd, copyContent, filesize);

    int fdFile = open(destination, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    
    if(write(fdFile, copyContent, strlen(copyContent))<0){
        perror("erreur sur l'écriture");
        return -1;
    }

    free(copyHeader);
    free(copyContent);
    close(fd);
    close(fdFile);
    return 0;
}

int cpOutsideTarInTar(char * archive, char * path, char * destination){
    print(path);
    int fd = openArchive (archive, O_RDWR);
    int fdFile = open(path, O_RDONLY);
    copyFileToTar (fd, fdFile, destination);
    close(fd);
    close(fdFile);
    return 0;
}

int cp1 (char * path, char * destination){
    char ** pathInTar = (char **) dividePathWithTar (path);
    char ** destinationInTar = (char **) dividePathWithTar (destination);
    return cpTarInTar(pathInTar[0], pathInTar[1], destinationInTar[1]);
}

int cp2 (char * path, char * destination){
    char ** pathInTar = (char **) dividePathWithTar (path);
    return cpTarInOutsideTar(pathInTar[0], pathInTar[1], destination);
}

int cp3 (char * path, char * destination){
    char ** destinationInTar = (char **) dividePathWithTar (destination);
    return cpOutsideTarInTar(destinationInTar[0], path, destinationInTar[1]);
}


int cp (char ** argv){
    if (getArgc(argv) > 3 && getArgc(argv) < 3){
        print("Trop d'arguments ou pas assez d'arguments!\n");
    }

    if (isARepertory(argv[1]) == 0){
         perror("utilisez l'option -r pour les dossiers");
        return -1;
    }

    int pathName = isInTar(argv[1]);
    int destination = isInTar(argv[2]);

    if(isARepertory(argv[2]) == 0){
        char * buf = malloc ( sizeof(char) * (strlen(argv[2]) + strlen(argv[1] + fileName (argv[1]))));
        sprintf(buf,"%s%s", argv[2], argv[1] + fileName (argv[1]) + 1);
        argv[2] = buf;
    }

    if(pathName == 0 && destination == 0)
        return cp1(argv[1], argv[2]);
    else if (pathName == 0 && destination == -1)
        return cp2(argv[1], argv[2]);
    else if (pathName == -1 && destination == 0) 
        return cp3(argv[1], argv[2]);
    else{
        executeCommandExterne(argv);
        return 0;
    }
}


char ** nameOfAllFileInDirectory (int fd, char * path, int archiveSize) {
    replaceCurseurToStart (fd);
    struct posix_header * h = malloc(BLOCKSIZE);
    char ** tabContent = malloc (sizeof(char *) * archiveSize);
    for (int i = 0; i < archiveSize; i++){
        tabContent[i] = NULL;
    }
    int i = 0;
    int tmp = getHeader(fd, h);
    while(tmp == 0) {
        tmp = getHeader(fd, h);
        if(isInRepertory (path, h -> name)){
            tabContent[i] = h -> name;
            i++;
        }
    }

    if(tmp == -2) {
        lseek(fd, -BLOCKSIZE, SEEK_CUR);
    }
    free(h);
    replaceCurseurToStart (fd);
    return tabContent;
}

int cpTarInTarOptionR(char * archive, char * path, char * destination){

    int fd = openArchive (archive, O_RDWR);
    int archiveSize = numberFileInArchive(fd);
    char ** tabContent = nameOfAllFileInDirectory(fd, path, archiveSize);

    for (int i = 0; i < archiveSize; i++){
        if(tabContent[i] == NULL){
            return 0;
        }else{
            cpTarInTar(archive, tabContent[i], destination);
        }
    }
    free(tabContent);
    close(fd);
    return 0;
}

int cpTarInOutsideTarOptionR(char * archive, char * path, char * destination){
    int fd = openArchive (archive, O_RDWR);
    int archiveSize = numberFileInArchive(fd);
    char ** tabContent = nameOfAllFileInDirectory(fd, path, archiveSize);
    for (int i = 0; i < archiveSize; i++){
        if(tabContent[i] == NULL){
            return 0;
        }else{
            cpTarInOutsideTar(archive, tabContent[i], destination);
        }
    }
    free(tabContent);
    close(fd);
    return 0;
}
int numberOfFileInDirectoryOutsideTar(DIR * dirp, char * path){

    if(dirp == NULL){
        perror("numberOfFileInDirectoryOutsideTar");
        return -1;
    }
    struct dirent *entity;
    entity = readdir(dirp);
    int count = 0;
    while(entity != NULL){
        if(strcmp(entity -> d_name, ".") != 0 && strcmp(entity -> d_name, "..") != 0){
            if(entity -> d_type == DT_DIR){     
                char * newPath = malloc (sizeof(char) * (strlen(path) + strlen(entity -> d_name) + 1));
                sprintf(newPath, "%s/%s", path, entity -> d_name);
                count += numberOfFileInDirectoryOutsideTar(opendir(newPath), newPath);
            }else{
                count += 1;
            }
        } 
        entity = readdir(dirp);
    }
    return count + 1;
}

void getAllFileNameFromFolderAux (DIR * dirp, char * path, int * index, char ** allEntityName){
    if(dirp == NULL){
        perror("getAllFileNameFromFolderAux");
    }
    struct dirent *entity;
    entity = readdir(dirp);
    while(entity != NULL){
        if(strcmp(entity -> d_name, ".") != 0 && strcmp(entity -> d_name, "..") != 0){

            if(entity -> d_type == DT_DIR){     
                char * newPath = malloc (sizeof(char) * (strlen(path) + strlen(entity -> d_name) + 1));
                sprintf(newPath, "%s/%s", path, entity -> d_name);
                allEntityName[*index] = newPath;
                (*index) ++;
                getAllFileNameFromFolderAux (opendir(newPath), newPath, index, allEntityName);
                free(newPath);
            }else{
                char * newPath = malloc (sizeof(char) * (strlen(path) + strlen(entity -> d_name) + 1));
                sprintf(newPath, "%s/%s", path, entity -> d_name);
                allEntityName[*index] = newPath;
                (*index) ++;
                free(newPath);
            }
        }
        entity = readdir(dirp);
    }
}

char ** getAllFileNameFromFolder (char * path){
    DIR * dirp = opendir(path);
    int index = 1;
    int size = numberOfFileInDirectoryOutsideTar(dirp, path);

    rewinddir(dirp);

    char ** allEntityName = malloc (sizeof(char * ) * size);
    getAllFileNameFromFolderAux (dirp, path, &index, allEntityName);
    
    allEntityName[0] = path;
    closedir(dirp);
    
    return allEntityName;
}
int cpOutsideTarInTarOptionR(char * archive, char * path, char * destination){
    DIR * dirp = opendir(path);

    int numberFile = numberOfFileInDirectoryOutsideTar(dirp, path);
    if(numberFile == 1){
        cpOutsideTarInTar(archive, path, destination);
    }else{

        char ** allEntityName = getAllFileNameFromFolder(path);
        char buf[2000];
        for (int i = 0; i < numberFile; i++){
            //char buf[strlen(allEntityName[i]) + strlen(destination) + 2];
            sprintf(buf, "%s/%s", destination, allEntityName[i]);
            //cpOutsideTarInTar(archive, allEntityName[i], buf);
            print(buf);
            memset(buf, '\0', 2000);
            print("\n");
        }
    }
    closedir(dirp);
    return 0;
}
