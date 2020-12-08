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

int mv (char ** argv){
    if (getArgc(argv) != 3){
        print("Trop d'arguments ou pas assez d'arguments!\n");
        return -1;
    }

    int source = isInTar(argv[1]);
    int destination = isInTar(argv[2]);

    //Source and destination are outside of tar
    if (source != 0 && destination != 0){
        executeCommandExterne(argv);
        return 0;
    }
    
    //First argument is a repertory
    if(isARepertory(argv[1]) == 0){
        print("AAAAAAAAAAA\n");
        if (source == 0)
            myRmdir_aux(argv[1]);
        else
            rmdir (argv[1]);

        if(destination == 0)
            myMkdir_aux(argv[2]);
        else
            mkdir (argv[2],O_RDWR);
        return 0;

    }else{
        cp(argv);
        if (source == 0)
            rm_aux(argv[1]);
        else
            unlink(argv[1]);
        
        return 0;
    }
}