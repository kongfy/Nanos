#include <stdio.h>
#include <unistd.h>

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
        MKDError error = mkdisk(path, 0);

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
