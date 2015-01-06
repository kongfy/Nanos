#include "stdio.h"
#include "unistd.h"
#include "const.h"

uint8_t buf[255];

int main(int argc, char *argv[])
{
    printf("Welcome to KongfyOS !\n");

    while (true) {
        printf("# ");

        int nread = read(STDIN_FILENO, buf, 255);
        buf[nread] = 0;

        int i;
        for (i = 0; i < nread; i ++) {
            if (buf[i] >= 'a' && buf[i] <= 'z') {
                buf[i] += 'A' - 'a';
            }
        }

        printf("GOT : %s\n", buf);
    }

    return 0;
}
