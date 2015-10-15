#include "stdio.h"
#include "unistd.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("mkdir: missing operand\n");
        return 1;
    }

    int i = 0;
    for (i = 1; i < argc; ++i) {
        int err = mkdir(argv[i]);

        if (err < 0) {
            if (-1 == err) {
                printf("mkdir: cannot create directory `%s': File exists\n", argv[i]);
                return 1;
            } else if (-2 == err) {
                printf("mkdir: cannot create directory `%s': No such file or directory\n", argv[i]);
                return 2;
            } else if (-3 == err) {
                printf("mkdir: cannot create directory `%s': Invalid directory name\n", argv[i]);
                return 3;
            } else {
                printf("mkdir: cannot create directory `%s': Unknown error\n", argv[i]);
                return 9;
            }
        }
    }

    return 0;
}
