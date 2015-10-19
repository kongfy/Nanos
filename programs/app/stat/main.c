#include "stdio.h"
#include "unistd.h"
#include "types.h"
#include "const.h"
#include "string.h"
#include "stat.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("stat: missing operand\n");
        return 1;
    }

    int i;
    for (i = 1; i < argc; ++i) {
        struct stat buf;

        if (stat(argv[i], &buf)) {
            printf("stat: cannot stat `%s': No such file or directory\n", argv[i]);
            continue;
        }

        printf("  File: `%s'\n", argv[i]);
        printf("  Size: %d    Blocks: %d    ", buf.size, buf.blks);
        if (buf.type == PLAIN) {
            printf("regular file\n");
        } else if (buf.type == DIRECTORY) {
            printf("directory\n");
        } else {
            printf("unknown\n");
        }
        printf("Device: %d    Inode: %d\n", buf.dev_id, buf.inode);
    }

    return 0;
}
