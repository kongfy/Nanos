#include "stdio.h"
#include "dirent.h"
#include "const.h"

static DIR dir;

int main(int argc, char *argv[])
{
    char *pathname;
    if (argc == 1) {
        pathname = ".";
    } else {
        pathname = argv[1];
    }

    int err = opendir(pathname, &dir);
    if (err < 0) {
        if (err == -1) {
            printf("ls: %s: No such file or directory\n", pathname);
        } else if (err == -3) {
            printf("ls: %s: Not a directory\n", pathname);
        }
    } else {
        struct dirent *entry;
        while ((entry = readdir(&dir)) != NULL) {
            printf("%s\n", entry->filename);
        }
    }

    return 0;
}
