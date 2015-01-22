#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "mkdisk.h"

#define MAX_LEN 1024

static
MKDError make_dir(const char *path)
{
    struct stat   statbuf;
    struct dirent *dirp;
    DIR           *dp;
    char          subpath[MAX_LEN];

    dp = opendir(path);

    while ((dirp = readdir(dp)) != NULL) {
        sprintf(subpath, "%s/%s", path, dirp->d_name);

        if (lstat(subpath, &statbuf) < 0) {
            return ESTAT;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
                continue;

            sprintf(subpath, "%s/%s", path, dirp->d_name);
            printf("path : %s\n", subpath);

            make_dir(subpath);
        } else if (S_ISREG(statbuf.st_mode)) {
            sprintf(subpath, "%s/%s", path, dirp->d_name);
            printf("file : %s\n", subpath);
        }
    }

    return 0;
}

MKDError mkdisk(const char *pathname, int fd)
{
    struct stat   statbuf;

    if (lstat(pathname, &statbuf) < 0) {
        return ESTAT;
    }

    if (S_ISDIR(statbuf.st_mode) == 0) {
        return ENDIR;
    }

    make_dir(pathname);

    return 0;
}
