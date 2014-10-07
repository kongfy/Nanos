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

int exec(int filename, char *const argv[])
{
    return syscall(SYS_EXEC, filename, argv);
}

void exit(int status)
{
    syscall(SYS_EXIT, status);
}
