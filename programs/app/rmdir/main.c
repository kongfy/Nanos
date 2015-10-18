#include "stdio.h"
#include "unistd.h"
#include "types.h"
#include "const.h"
#include "string.h"
#include "stat.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("rmdir: missing operand\n");
        return 1;
    }

    int i;
    for (i = 1; i < argc; ++i) {
        struct stat buf;

        if (stat(argv[i], &buf)) {
            printf("rmdir: failed to remove `%s': No such file or directory\n", argv[i]);
            return 1;
        }

        if (buf.type == PLAIN) {
            printf("rmdir: failed to remove `%s': Not a directory\n", argv[i]);
            return 1;
        } else if (buf.type == DIRECTORY) {
            int err = rmdir(argv[i]);

            if (err < 0) {
                if (-3 == err) {
                    printf("rmdir: failed to remove `%s': Directory not empty\n", argv[i]);
                } else if (-4 == err) {
                    printf("rmdir: failed to remove `%s': Invalid argument\n", argv[i]);
                } else {
                    printf("rmdir: failed to remove `%s': Unknown error\n", argv[i]);
                }
                return 1;
            }
        }
    }

    return 0;
}
