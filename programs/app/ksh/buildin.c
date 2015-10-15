#include "types.h"
#include "stdio.h"
#include "string.h"
#include "ksh.h"
#include "unistd.h"

static
void cd(int argc, char *argv[])
{
    if (argc < 2) {
        return;
    }

    char *path = argv[1];
    if (path == NULL) {
        return;
    }

    while (*path != '\0' && is_blank_char(*path)) *(path++) = '\0';
    int len = strlen(path);
    while (len > 0 && is_blank_char(*path)) path[len-- - 1] = '\0';

    int err = chdir(path);
    if (err == -1) {
        printf("-ksh: %s: No such file or directory\n", path);
    } else if (err == -3) {
        printf("-ksh: %s: Not a directory\n", path);
    }
}

bool buildin(int argc, char *argv[])
{
    char *cmd = argv[0];

    if (strcmp(cmd, "cd") == 0) {
        cd(argc, argv);
        return true;
    }

    return false;
}
