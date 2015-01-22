#include "stdio.h"
#include "syscall.h"

int printk(const char *format, ...)
{
    // TODO: implement me!
    syscall(SYS_PRINTK, &format);
    return 0;
}
