#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int lsPath(const char *path) {
    DIR *d = opendir(path);
    if(!d) {
        perror("opendir");
        return -1;
    }

    struct dirent *dir;
    while((dir = readdir(d))) {
        printf("%s\n", dir->d_name);
    }
    closedir(d);

    return 0;
}

int ls(int argc, char *argv[]) {
    int returnValue = 0;

    if(argc == 1) {
        if(lsPath(".") != 0) {
            return -1;
        }
    } else {
        for(int i = 1; i < argc; i++) {
            printf("%s:\n", argv[i]);
            if(lsPath(argv[i]) != 0) {
                returnValue = -1;
            }
            printf("\n");
        }
    }

    return returnValue;
}
