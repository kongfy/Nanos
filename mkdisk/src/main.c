#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "mkdisk.h"

int main(int argc, char *argv[])
{
    int ch;

    char *path = NULL;
    while ((ch = getopt(argc, argv, "p:")) != -1) {
        switch (ch) {
        case 'p':
            path = optarg;
        }
    }

    if (path) {
        int fd = open("disk", O_RDWR | O_CREAT | O_TRUNC | O_SYNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        MKDError error = mkdisk(path, fd);
        close(fd);

        char *errmsg = NULL;
        switch (error) {
        case ENDIR:
            errmsg = "not a directory";
        case ESTAT:
            errmsg = "stat error";
        default:
            errmsg = NULL;
        }

        if (errmsg) {
            printf("ERROR : %s\n", errmsg);
        }
    }

    return 0;
}
