#include "stdio.h"
#include "unistd.h"
#include "types.h"
#include "const.h"
#include "string.h"
#include "stat.h"

#define BUFLEN 256

static struct stat buf;
static uint8_t fbuf[BUFLEN];

static
int cat(char *path)
{
    if (stat(path, &buf)) {
        printf("cat: %s: No such file or directory\n", path);
        return 1;
    }

    if (buf.type == PLAIN) {
        int fd = open(path);
        int len;

        while ((len = read(fd, fbuf, BUFLEN)) > 0) {
            write(STDOUT_FILENO, fbuf, len);
        }

        close(fd);

        if (len < 0) {
            printf("cat: %s: Error\n", path);
            return 1;
        }
    } else if (buf.type == DIRECTORY) {
        printf("cat: %s: Is a directory\n", path);
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("cat: missing operand\n");
        return 1;
    }

    int i = 0, j = 0;
    for (i = 1; i < argc; ++i) {
        if ('-' == argv[i][0]) {
            int len = strlen(argv[i]);
            for (j = 1; j < len; ++j) {
                printf("cat: invalid option -- '%c'\n", argv[i][j]);
                return 1;
            }
        }
    }

    bool flag = false;
    for (i = 1; i < argc; ++i) {
        if ('-' != argv[i][0]) {
            int ret = cat(argv[i]);
            if (ret != 0) return ret;
            flag = true;
        }
    }

    if (!flag) {
        printf("rm: missing operand\n");
        return 1;
    }

    return 0;
}
