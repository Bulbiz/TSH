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

//FIXME : manque la vérification si le path est un fichier ou pas
int rmInTar(char * archive, char * path){
    int fd = openArchive (archive, O_RDWR);

    /* Obtenir la taille du contenu de l'archive APRES le fichier */
    size_t size = getSizeAfterFile (path, fd);

    /* Obtenir la taille du fichier a supprimer (header + contenu) */
    struct posix_header * buf = malloc (512);
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

int main(int argc, char * argv[]){
    rmInTar("archive.tar","rep/toto");
    return 0;
}
