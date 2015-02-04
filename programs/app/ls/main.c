#include "stdio.h"
#include "dirent.h"

DIR g_dir;

int main(int argc, char *argv[])
{
    char *pathname;
    if (argc == 0) {
        pathname = ".";
    } else {
        pathname = argv[0];
    }

    int err = opendir(pathname, &g_dir);
    if (err < 0) {
        if (err == -1) {
            printf("ls: %s: No such file or directory\n", pathname);
        } else if (err == -3) {
            printf("ls: %s: Not a directory\n", pathname);
        }
    } else {
        struct dirent *entry;
        while ((entry = readdir(&g_dir))) {
            printf("%s\n", entry->filename);
        }
    }

    return 0;
}
