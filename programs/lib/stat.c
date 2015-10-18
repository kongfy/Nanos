#include "stat.h"
#include "syscall.h"

int stat(const char *path, struct stat *buf)
{
    return syscall(SYS_STAT, path, buf);
}
