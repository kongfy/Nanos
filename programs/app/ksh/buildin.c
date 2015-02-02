#include "types.h"
#include "stdio.h"
#include "const.h"
#include "string.h"

bool buildin(char *filename, char *argv[])
{
    if (strcmp(filename, "cd") == 0) {
        printf("I'm cd, please implement me!\n");
        return true;
    }

    return false;
}
