#include "stdio.h"
#include "dirent.h"
#include "types.h"

char buf[MAX_PATH_LEN];

int main(int argc, char *argv[])
{
    char *path = getcwd(buf, MAX_PATH_LEN);

    if (!path) {
        printf("pwd: Unexpected Error.\n");
    } else {
        printf("%s\n", path);
    }

    return 0;
}
