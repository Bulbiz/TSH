#include <unistd.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include "tar.h"
#include "functionInTar.h"
#include "pathTreatement.h"

/* Print the values of environment variable PWD */
void pwd (){
    print(cwd);
    print("\n");
}


