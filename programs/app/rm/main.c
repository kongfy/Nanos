#include "stdio.h"
#include "unistd.h"
#include "types.h"
#include "const.h"
#include "string.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("rm: missing operand\n");
        return 1;
    }

    bool recursive = false;
    int i = 0, j = 0;
    for (i = 1; i < argc; ++i) {
        if ('-' == argv[i][0]) {
            int len = strlen(argv[i]);
            for (j = 1; j < len; ++j) {
                if ('r' == argv[i][j]) {
                    recursive = true;
                } else {
                    printf("rm: invalid option -- '%c'\n", argv[i][j]);
                    return 1;
                }
            }
        }
    }

    bool flag = false;
    for (i = 1; i < argc; ++i) {
        if ('-' != argv[i][0]) {
            flag = true;
        }
    }

    if (!flag) {
        printf("rm: missing operand\n");
        return 1;
    }

    printf("recursive : %d\n", recursive);

    return 0;
}
