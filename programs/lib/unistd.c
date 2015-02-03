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

int exec(const char *filename, char *const argv[])
{
    return syscall(SYS_EXEC, filename, argv);
}

int waitpid(pid_t pid)
{
    return syscall(SYS_WAITPID, pid);
}

unsigned int sleep(unsigned int seconds)
{
    return syscall(SYS_SLEEP, seconds);
}

void exit(int status)
{
    syscall(SYS_EXIT, status);
}

int open(const char *filename)
{
    return syscall(SYS_OPEN, filename);
}

int lseek(int fd, int offset, int whence)
{
    return syscall(SYS_LSEEK, fd, offset, whence);
}

int close(int fd)
{
    return syscall(SYS_CLOSE, fd);
}

int dup(int oldfd)
{
    return syscall(SYS_DUP, oldfd);
}

int dup2(int oldfd, int newfd)
{
    return syscall(SYS_DUP2, oldfd, newfd);
}

int pipe(int pipefd[2])
{
    return syscall(SYS_PIPE, pipefd);
}

int read(int fd, uint8_t *buf, int len)
{
    return syscall(SYS_READ, fd, buf, len);
}

int write(int fd, uint8_t *buf, int len)
{
    return syscall(SYS_WRITE, fd, buf, len);
}

int chdir(const char *path)
{
    return syscall(SYS_CHDIR, path);
}
