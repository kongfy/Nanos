#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "mkdisk.h"
#include "fsys.h"

#define MAX_LEN 1024

extern int fd_out;

static
MKDError make_dir(const char *path, const char *filename, int parent_inode)
{
    struct stat   statbuf;
    struct dirent *dirp;
    DIR           *dp;
    char          subpath[MAX_LEN];
    MKDError      err;

    dp = opendir(path);
    int inode = make_sub_dir(filename, parent_inode);

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

            if ((err = make_dir(subpath, dirp->d_name, inode)) < 0) {
                return err;
            };
        } else if (S_ISREG(statbuf.st_mode)) {
            sprintf(subpath, "%s/%s", path, dirp->d_name);
            printf("file : %s\n", subpath);

            make_sub_file(subpath, dirp->d_name, inode);
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

    if (chdir(pathname) < 0) {
        return ECHDIR;
    }

    fd_out = fd;

    init_disk();
    make_dir(".", "/", -1);

    return 0;
}
