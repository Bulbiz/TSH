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

void mv (char ** argv){
    if (getArgc(argv) != 3)
        print("Trop d'arguments ou pas assez d'arguments!\n");  
    if(cp (argv) == 0)
        rm(argv[1]);
    else
        print("Je n'ai pas reussi à copier le fichier, echec du move\n");  
}