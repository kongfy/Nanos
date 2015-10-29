#include "stdio.h"
#include "unistd.h"
#include "types.h"
#include "const.h"
#include "string.h"
#include "stat.h"
#include "dirent.h"

static struct stat buf;

static
int touch(char *path)
{
    if (stat(path, &buf)) {
        int fd = open(path);

        if (fd >= 0) {
            close(fd);
        } else {
            if (-2 == fd) {
                printf("touch: cannot touch `%s': No such file or directory\n", path);
            } else if (-3 == fd) {
                printf("touch: cannot touch `%s': Invalid file name\n", path);
            }

            return 1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("touch: missing file operand\n");
        return 1;
    }

    int i = 0, j = 0;
    for (i = 1; i < argc; ++i) {
        if ('-' == argv[i][0]) {
            int len = strlen(argv[i]);
            for (j = 1; j < len; ++j) {
                printf("touch: invalid option -- '%c'\n", argv[i][j]);
                return 1;
            }
        }
    }

    bool flag = false;
    for (i = 1; i < argc; ++i) {
        if ('-' != argv[i][0]) {
            int ret = touch(argv[i]);
            if (ret != 0) return ret;
            flag = true;
        }
    }

    if (!flag) {
        printf("touch: missing file operand\n");
        return 1;
    }

    return 0;
}
