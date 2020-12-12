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

/* Return 0 is there is file in the folder else -1 */
int fileInFolderInTar (char * archive, char * path){
    int fd = openArchive (archive, O_RDONLY);
    struct posix_header * buf = malloc (512);
    while (getHeader(fd,buf) == 0){
        if(strstr(buf->name, path + '/') > 0)
            return 0;
        else
            passContent(fd, buf);
    }
    return -1;
}   

/*  Execute rmdir inside a tar
    Return 0 if the command succes, else -1 */
int rmdirInTar(char * archive, char * path){
    int fd = openArchive(archive, O_RDWR);
    struct posix_header * buf = malloc (512);
    if(searchFile(fd, buf, path) == 0 && buf->typeflag == '5' && fileInRepertory(fd, path) == -1){
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
        print("impossible de supprimer le répertoire");
        return -1;
    }
}

/* Divide the path for the execution of rmdir inside a tarball */
int myRmdir_aux (char * path){
    char ** pathInTar = (char **) dividePathWithTar (path);
    return rmdirInTar(pathInTar[0], pathInTar[1]);
}

/* Execute the command rmdir */
void myRmdir (char ** argv){
    if(isInTar(argv[1]) == 0)
        myRmdir_aux (argv[1]);
    else
        executeCommandExterne(argv);
}