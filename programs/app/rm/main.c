#include "stdio.h"
#include "unistd.h"
#include "types.h"
#include "const.h"
#include "string.h"
#include "stat.h"
#include "dirent.h"

static struct stat buf; // try save stack memory, BE CAREFUL!

static
int remove(char *path, bool recursive)
{
    if (stat(path, &buf)) {
        printf("rm: cannot remove `%s': No such file or directory\n", path);
        return 1;
    }

    if (buf.type == PLAIN) {
        int err = unlink(path);
        if (err != 0) {
            printf("rm: cannot remove `%s': Unknown error\n", path);
            return 1;
        }
    } else if (buf.type == DIRECTORY) {
        if (!recursive) {
            printf("rm: cannot remove `%s': Is a directory\n", path);
            return 1;
        }

        DIR dir;
        int err = opendir(path, &dir);

        if (err < 0) {
            return 1;
        } else {
            char buf[MAX_PATH_LEN];
            char *pwd = getcwd(buf, MAX_PATH_LEN);
            chdir(path);

            struct dirent *entry;
            while ((entry = readdir(&dir)) != NULL) {
                if (strcmp(entry->filename, ".") == 0
                    || strcmp(entry->filename, "..") == 0) {
                    continue;
                }

                int ret = remove(entry->filename, recursive);

                if (0 != ret) {
                    return ret;
                }
            }

            chdir(pwd);
            int err = rmdir(path);
            if (err < 0) {
                if (-3 == err) {
                    printf("rm: failed to remove `%s': Directory not empty\n", path);
                } else if (-4 == err) {
                    printf("rm: failed to remove `%s': Invalid argument\n", path);
                } else {
                    printf("rm: failed to remove `%s': Unknown error\n", path);
                }
                return 1;
            }

        }
    }

    return 0;
}

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
            int ret = remove(argv[i], recursive);
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
