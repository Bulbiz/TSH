#include "pathTreatement.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
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
#include "rmdir.h"

/* Remove a file without any verification */
int removeFile (char * archive, char * path){
    int fd = openArchive (archive, O_RDWR);
    struct posix_header * buf = malloc (512);
    if (searchFile(fd, buf, path) == -1){
        print("Error: No such file !\n");
        close(fd);
        free(buf);
        return -1;
    }

    /* Obtenir la taille du contenu de l'archive APRES le fichier */
    size_t size = getSizeAfterFile (path, fd);

    /* Obtenir la taille du fichier a supprimer (header + contenu) */
    size_t fileSize = searchFileSize (fd, buf, path);

    /* Sauvegarder le contenu de l'archive qui est APRES le fichier */
    char * contentAfterFile = (char *) malloc (sizeof(char)* size);
    contentAfterFile = getContentUntilPathFile(path, fd, size);
        
    /* Placer le curseur juste devant le fichier a supprimer */
    searchFile(fd, buf, path);
    lseek(fd, -BLOCKSIZE, SEEK_CUR); 

    /* Supprimer TOUT le contenu du tar à partir de fichier*/
    char * tmp0 = (char *) malloc (sizeof(char)* (size + fileSize));
    memset(tmp0, 0, size + fileSize);
     write(fd, tmp0, size + fileSize);

    /* Réecrire le contenu sauvegarder a l'endroit du fichier */
    passArchive(fd);
    write (fd, contentAfterFile, size);
        
    /* Free ce qui soit être free */
    free(contentAfterFile);
    free(buf);
    free(tmp0);
    close(fd);

    return 0;
}

/* Remove a file if it's a regular file */
int rmInTar(char * archive, char * path){
    int fd = openArchive (archive, O_RDWR);
    struct posix_header * buf = malloc (512);
    searchFile(fd, buf, path);
    if(buf->typeflag == '0'){
        /* Obtenir la taille du contenu de l'archive APRES le fichier */
        size_t size = getSizeAfterFile (path, fd);

        /* Obtenir la taille du fichier a supprimer (header + contenu) */
        size_t fileSize = searchFileSize (fd, buf, path);

        /* Sauvegarder le contenu de l'archive qui est APRES le fichier */
        char * contentAfterFile = (char *) malloc (sizeof(char)* size);
        contentAfterFile = getContentUntilPathFile(path, fd, size);
        
        /* Placer le curseur juste devant le fichier a supprimer */
        searchFile(fd, buf, path);
        lseek(fd, -BLOCKSIZE, SEEK_CUR); 

        /* Supprimer TOUT le contenu du tar à partir de fichier*/
        char * tmp0 = (char *) malloc (sizeof(char)* (size + fileSize));
        memset(tmp0, 0, size + fileSize);
        write(fd, tmp0, size + fileSize);

        /* Réecrire le contenu sauvegarder a l'endroit du fichier */
        passArchive(fd);
        write (fd, contentAfterFile, size);
        
        /* Free ce qui soit être free */
        free(contentAfterFile);
        free(buf);
        free(tmp0);
        close(fd);
        return 0;
    }else{
        perror("impossible de supprimer, ce n'est pas un fichier");
        return -1;
    }

}


int rmReccursiveInTar (char * archive, char * path){
    int fd = openArchive (archive, O_RDWR);
    struct posix_header * buf = malloc (512);

    if(searchFile(fd,buf,path) == -1){
        print("Error : No such File !\n");
        return -1;
    }

    if(buf -> typeflag == '0'){
        rmInTar(archive,buf -> name);
        return 0;
    }

    replaceCurseurToStart(fd);
    while (getHeader(fd,buf) == 0){
        if(strncmp(path, buf -> name,strlen(path)) == 0){
                replaceCurseurToStart(fd);
                removeFile(archive,buf -> name);
        }
        
    }
    close(fd);
    free (buf);
    return 0;
}

void rmR (char * path){
    char ** pathInTar = (char **) dividePathWithTar (path);
    rmReccursiveInTar (pathInTar[0], pathInTar[1]);
}

void rm (char * path){
    char ** pathInTar = (char **) dividePathWithTar (path);
    //rmInTar(pathInTar[0], pathInTar[1]);
    rmReccursiveInTar (pathInTar[0], pathInTar[1]);
}


/*int main(int argc, char * argv[]){
    rmInTar("archive.tar","rep/toto");
    return 0;
}*/
