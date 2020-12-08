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
#include "rm.h"
#include "cp.h"
#include "mkdir.h"
#include "rmdir.h"

void mvFile (char ** argv) {
    if(cp (argv) == 0)
        rm(argv);
    else
        print("Echec du move, je n'ai pas réussi à déplacer le fichier\n");  
}

void mvRepertoire (char ** argv){
    if(rmdir(argv[0]) == 0)
        myMkdir(argv);
    else
        print("Echec du move, je n'ai pas réussi à déplacer le répertoire\n");
}

void mv (char ** argv){
    if (getArgc(argv) != 3)
        print("Trop d'arguments ou pas assez d'arguments!\n");
    
    if (isARepertory(argv[1]) == 0)
        mvRepertoire (argv);
    else
        mvFile(argv);  
}