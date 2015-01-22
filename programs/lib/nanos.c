#include "syscall.h"
#include "unistd.h"

int main();
int main(int argc, char *argv[]);

void _start(int argc, char *argv[])
{
    exit(main(argc, argv));
}
