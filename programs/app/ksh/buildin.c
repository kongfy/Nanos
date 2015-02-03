#include "types.h"
#include "stdio.h"
#include "string.h"
#include "ksh.h"
#include "unistd.h"

static
void cd(char *path)
{
    if (path == NULL) {
        return;
    }

    while (path != '\0' && !isPathChar(*path)) *(path++) = '\0';
    int len = strlen(path);
    while (len > 0 && !isPathChar(path[len-1])) path[len-- - 1] = '\0';

    int err = chdir(path);
    if (err == -1) {
        printf("-ksh: %s: No such file or directory\n", path);
    } else if (err == -3) {
        printf("-ksh: %s: Not a directory\n", path);
    }
}

bool buildin(char *filename, char *argv[])
{
    if (strcmp(filename, "cd") == 0) {
        cd(argv[0]);
        return true;
    }

    return false;
}
