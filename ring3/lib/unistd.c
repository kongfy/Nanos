#include "unistd.h"
#include "syscall.h"


pid_t getpid(void)
{
    return syscall(SYS_GETPID);
}

pid_t fork(void)
{
    return syscall(SYS_FORK);
}
