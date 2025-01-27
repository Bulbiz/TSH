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
#include "mkdir.h"

/* give the index for the name after the last '/' */
int fileName (char * pathName){
    int i = strlen(pathName) -2;
    while(pathName[i] != '/')
        i--;
    return i;
}

/*  Execute the command cp if the source and destination is in the tar 
    Destination is the name of the header (it needs to have the full path with archive as it's racine) 
    Path is the name of the file in the tar that needs to be copied 
    sameArchive return 0 if archivePath and archiveDestination is the same else -1*/
int cpTarInTar(char * archivePath, char * archiveDestination, char * path, char * destination){

    int fdPath = openArchive (archivePath, O_RDWR);
    int sameArchive = 0;
    int fdDestination = 0;
    if(strcmp (archivePath, archiveDestination) != 0){
        sameArchive = -1;
        fdDestination = openArchive (archiveDestination, O_RDWR);
    }
    struct posix_header * copyHeader = malloc (BLOCKSIZE);
    if(searchFile (fdPath, copyHeader, path) == -1){
        print("fichier inexistant\n");
        return -1;
    }
  
    memset(copyHeader -> name, 0, 100);
    memcpy(copyHeader -> name, destination, (strlen(destination)));
    set_checksum(copyHeader);
    if(!check_checksum(copyHeader)){
        print("impossible d'initialiser checksum\n");
        return -1;
    }

    int filesize;
    sscanf(copyHeader -> size, "%o" , &filesize);
    char * copyContent = malloc (sizeof(char) * filesize);
    if(sameArchive == 0){
        read (fdPath, copyContent, filesize);
        passArchive(fdPath);

        write(fdPath, copyHeader, BLOCKSIZE);
        write(fdPath, copyContent, filesize);
    }else{
        read (fdPath, copyContent, filesize);
        passArchive(fdDestination);

        write(fdDestination, copyHeader, BLOCKSIZE);
        write(fdDestination, copyContent, filesize);
        close(fdDestination);
    }

    free(copyHeader);
    free(copyContent);
    close(fdPath);
    return 0;
}
/*  Execute the command cp if the source is in the tar and destination is outside of the tar
    Destination is the name of the header (it needs to have the full path with archive as it's racine) 
    Path is the name of the file in the tar that needs to be copied
*/
int cpTarInOutsideTar(char * archive, char * path, char * destination){

    int fd = openArchive (archive, O_RDONLY);

    struct posix_header * copyHeader = malloc (BLOCKSIZE);
    if(searchFile (fd, copyHeader, path) == -1){
        print("fichier inexistant\n");
        return -1;
    }

    int filesize;
    sscanf(copyHeader -> size, "%o" , &filesize);

    char * copyContent = malloc (sizeof(char) * (filesize + 2));
    memset(copyContent,'\0',filesize + 2);
    read (fd, copyContent, filesize);

    int fdFile = open(destination, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if(write(fdFile, copyContent, strlen(copyContent)) == -1){
        perror("cpTarInOutsideTar");
        return -1;
    }

    free(copyHeader);
    free(copyContent);
    close(fd);
    close(fdFile);
    return 0;
}
/*  Execute the command cp if the source is outside of the tar and destination is in the tar
    Destination is the name of the header (it needs to have the full path with archive as it's racine) 
    Path is the name of the file in the tar that needs to be copied
*/
int cpOutsideTarInTar(char * archive, char * path, char * destination){
    int fd = openArchive (archive, O_RDWR);
    int fdFile = open(path, O_RDONLY);
    copyFileToTar (fd, fdFile, destination);
    close(fd);
    close(fdFile);
    return 0;
}

/* Divide the path and the destination cp if the source and destination is in the tar*/
int cp1 (char * path, char * destination){
    char ** pathInTar = (char **) dividePathWithTar (path);
    char ** destinationInTar = (char **) dividePathWithTar (destination);

    return cpTarInTar(pathInTar[0], destinationInTar[0], pathInTar[1], destinationInTar[1]);
}

/* Divide the path and the destination cp if the source is in the tar and destination is outside of the tar */
int cp2 (char * path, char * destination){
    char ** pathInTar = (char **) dividePathWithTar (path);
    return cpTarInOutsideTar(pathInTar[0], pathInTar[1], destination);
}

/* Divide the path and the destination cp if the source is outside of the tar and destination is in the tar */
int cp3 (char * path, char * destination){
    char ** destinationInTar = (char **) dividePathWithTar (destination);
    return cpOutsideTarInTar(destinationInTar[0], path, destinationInTar[1]);
}

/* Execute the command cp */
int cp (char ** argv){
    if (getArgc(argv) != 3){
        print("Trop d'arguments ou pas assez d'arguments!\n");
    }

    if (isARepertory(argv[1]) == 0){
         print("action impossible sur un dossier, utilisez l'option -r pour les dossiers\n");
        return -1;
    }
    
    if (fileExist(argv[1]) != 0){
        print ("Source introuvable, Impossible de Copier\n");
        return -1;
    }

    int pathName = isInTar(argv[1]);
    int destination = isInTar(argv[2]);

    if (!isArchiveRacine (getFileRepertory(argv[2])) == 0 && fileExist(getFileRepertory(argv[2])) != 0){
        print ("Chemin non valide !\n");
        return -1;
    }

    if(isARepertory(argv[2]) == 0 || isArchiveRacine (argv[2]) == 0){
        argv[2] = addSlash (argv[2]);
        char * buf = malloc ( sizeof(char) * (strlen(argv[2]) + strlen(argv[1] + fileName (argv[1]))));
        sprintf(buf,"%s%s", argv[2], argv[1] + fileName (argv[1]) + 1);
        argv[2] = buf;
    }

    if (fileExist(argv[2]) == 0){
        print ("Fichier déja présent à la destination ou chemin non valide, Impossible de Copier\n");
        return -1;
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

/* Make a list of all the file inside a directory */
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
        if(strcmp(path,h->name) == 0 || (strlen (path) < strlen(h -> name) && strncmp(path,h -> name,strlen(path)) == 0)){
            tabContent[i] = duplicate(h -> name);
            i++;
        }
        tmp = getHeader(fd, h);
    }

    if(tmp == -2) {
        lseek(fd, -BLOCKSIZE, SEEK_CUR);
    }
    free(h);
    replaceCurseurToStart (fd);
    return tabContent;
}

/*sort tab from the number of slash */
char ** sortInsertionBySlash (char ** tab, int size){
    for (int i = 0; i < size - 1; i++){
        for (int j = i + 1; j < size; j++){
            if(numberOfSlash(tab[i]) > numberOfSlash(tab[j])){
                char * tmp = tab[i];
                tab[i] = tab[j];
                tab[j] = tmp;
                break;
            } 
        }
    }
    return tab;
}

/*sorts the repertories by the number of slashes in their name and places them at the beginning of tab
 then sorts the remaining files and at the end, filled by NULL the remaining places*/
char ** sortTabWithDirectoryFirst(char ** tab, int sizeArchive){
    int countDirectory = 0;
    for(int i = 0; i < sizeArchive; i++){
        if(tab[i] != NULL && strcmp (tab[i] + (strlen(tab[i]) -1) , "/") == 0){
            countDirectory++;
        }
    }
    char ** tabDirectory = malloc (sizeof(char *) * countDirectory);
    for (int i = 0; i < countDirectory; i++){
        tabDirectory[i] = NULL;
    }
    int index = 0;
    for(int i = 0; i < sizeArchive; i++){
        if(tab[i] != NULL && strcmp (tab[i] + (strlen(tab[i]) -1) , "/") == 0){
            tabDirectory[index] = tab[i];
            index++;
        }
    }
    char ** tmpTabDirectory = sortInsertionBySlash (tabDirectory, countDirectory);
    char ** tabContentSort = malloc (sizeof(char *) * sizeArchive);

    for (int i = 0; i < sizeArchive; i++){
        tabContentSort[i] = NULL;
    }

    for (int i = 0; i < countDirectory; i++){
        tabContentSort[i] = tmpTabDirectory[i];
    }
    int index2 = countDirectory;
    for(int i = 0; i< sizeArchive; i++){
        if(tab[i] != NULL && strcmp (tab[i] + (strlen(tab[i]) -1) , "/") != 0){
            tabContentSort[index2] = tab[i];
            index2++;
        }
    }
    return tabContentSort;
}

/* Execute the command cp with the option -r if the source and destination is in the tar */
int cpTarInTarOptionR(char * archivePath, char * archiveDestination, char * path, char * destination){
    int fd = openArchive (archivePath, O_RDWR);
    int archiveSize = numberFileInArchive(fd);
    char ** tabContent = nameOfAllFileInDirectory(fd, path, archiveSize);
    for (int i = 0; i < archiveSize; i++){
        if(tabContent[i] == NULL){
            return 0;
        }else{
            char * buf = malloc (sizeof(char) * (strlen(tabContent[i]) + strlen(destination) + 5));
            sprintf(buf,"%s%s",destination,tabContent[i]);
            cpTarInTar(archivePath, archiveDestination, tabContent[i], buf);
        }
    }
    free(tabContent);
    close(fd);
    return 0;
}

/* Execute the command cp with the option -r if the source is in the tar and destination is outside the tar */
int cpTarInOutsideTarOptionR(char * archive, char * path, char * destination){
    int fd = openArchive (archive, O_RDWR);
    int archiveSize = numberFileInArchive(fd);
    char ** tabContent = nameOfAllFileInDirectory(fd, path, archiveSize);
    char ** sortTabContent = sortTabWithDirectoryFirst(tabContent, archiveSize);
    for (int i = 0; i < archiveSize; i++){
        if(sortTabContent[i] == NULL){
            return 0;
        }else{
            char * buf = malloc (sizeof(char) * (strlen(sortTabContent[i]) + strlen(destination) + 10));
            sprintf(buf,"%s%s",addSlash(destination),sortTabContent[i]);
            if (isARepertory(combineArchiveAndPath(archive,sortTabContent[i])) == 0){ 
                mkdir(buf, S_IRWXU | S_IRWXO| S_IRWXG);
            }else{
                cpTarInOutsideTar(archive, sortTabContent[i], buf);
            }
        }
    }
    free(tabContent);
    close(fd);
    return 0;
}

/* Count the number of file in a repertory */
int numberOfFileInDirectoryOutsideTar(DIR * dirp, char * path){

    if(dirp == NULL){
        perror("numberOfFileInDirectoryOutsideTar");
        return 0;
    }
    struct dirent *entity;
    entity = readdir(dirp);
    int count = 0;
    while(entity != NULL){
        if(strcmp(entity -> d_name, ".") != 0 && strcmp(entity -> d_name, "..") != 0){
            if(entity -> d_type == DT_DIR){     
                char * newPath = malloc (sizeof(char) * (strlen(path) + strlen(entity -> d_name) + 2));
                memset(newPath ,'\0',strlen(path) + strlen(entity -> d_name) + 1);
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

/* Auxilary function : List all the file that is contained in the dirp repertory */
void getAllFileNameFromFolderAux (DIR * dirp, char * path, int * index, char ** allEntityName){
    if(dirp == NULL){
        perror("getAllFileNameFromFolderAux");
    }
    struct dirent *entity;
    entity = readdir(dirp);
    while(entity != NULL){
        if(strcmp(entity -> d_name, ".") != 0 && strcmp(entity -> d_name, "..") != 0){
            char * newPath = malloc (sizeof(char) * (strlen(path) + strlen(entity -> d_name) + 2));
            memset(newPath, '\0', strlen(path) + strlen(entity -> d_name) + 1);
            path = addSlash(path);
            sprintf(newPath, "%s%s", path, entity -> d_name);
            allEntityName[*index] = newPath;
            (*index) ++;

            if(entity -> d_type == DT_DIR){     
                getAllFileNameFromFolderAux (opendir(newPath), newPath, index, allEntityName);
            }
        }
        entity = readdir(dirp);
    }
}

/* List all the file that is contained in the path repertory */
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

/* Execute the command cp with the option -r if the source is is outside of the tar and destination in the tar */
int cpOutsideTarInTarOptionR(char * archive, char * path, char * destination){
    DIR * dirp = opendir(path);

    int numberFile = numberOfFileInDirectoryOutsideTar(dirp, path);

    if(numberFile == 1){
        cpOutsideTarInTar(archive, path, destination);
    }else{
        char ** allEntityName = getAllFileNameFromFolder(path);
        for (int i = 0; i < numberFile; i++){
            char * buf = malloc (strlen(allEntityName[i]) + strlen(destination) + 3);
            if (strlen(destination) == 0){
                sprintf(buf, "%s",allEntityName[i] + fileName(path) + 1);
            }else
                sprintf(buf, "%s%s", destination, allEntityName[i] + fileName(path) + 1);

            if (isARepertory (allEntityName[i]) == 0){
                mkdirInTar(archive,buf);
            }else{
                cpOutsideTarInTar(archive, allEntityName[i], buf);
            }
        }
    }
    closedir(dirp);
    return 0;
}

/* Divide the path and the destination cp -r if the source and destination is in the tar*/
int cp1R (char * path, char * destination){
    char ** pathInTar = (char **) dividePathWithTar (path);
    char ** destinationInTar = (char **) dividePathWithTar (destination);
    return cpTarInTarOptionR(pathInTar[0], destinationInTar[0], pathInTar[1], destinationInTar[1]);
}

/* Divide the path and the destination cp -r if the source is in the tar and destination is outside of the tar */
int cp2R (char * path, char * destination){
    char ** pathInTar = (char **) dividePathWithTar (path);
    return cpTarInOutsideTarOptionR(pathInTar[0], pathInTar[1], destination);
}

/* Divide the path and the destination cp -r if the source is outside of the tar and destination is in the tar */
int cp3R (char * path, char * destination){
    char ** destinationInTar = (char **) dividePathWithTar (destination);
    return cpOutsideTarInTarOptionR(destinationInTar[0], path, destinationInTar[1]);
}

/* Execute the command cp -r*/
int cpR (char ** argv){
    if (getArgc(argv) != 3){
        print("Trop d'arguments ou pas assez d'arguments!\n");
    }

    argv[1] = addSlash(argv[1]);
    argv[2] = addSlash(argv[2]);

    if (isARepertory(argv[1]) == -1){
         print("action impossible, la source n'est pas un dossier, utilisez cp sans option\n");
        return -1;
    }
    if (isARepertory(argv[2]) == -1){
         print("action impossible,la destination n'est pas un dossier, utilisez cp sans option\n");
        return -1;
    }
    int pathName = isInTar(argv[1]);
    int destination = isInTar(argv[2]);

    if(pathName == 0 && destination == 0)
        return cp1R(argv[1], argv[2]);
    else if (pathName == 0 && destination == -1)
        return cp2R(argv[1], argv[2]);
    else if (pathName == -1 && destination == 0) 
        return cp3R(argv[1], argv[2]);
    else{
        argv[getArgc(argv)] = "-r";
        executeCommandExterne(argv);
        return 0;
    }
}
