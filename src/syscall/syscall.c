#include "syscall.h"
#include "kernel.h"

int printk(char **formatp);
pid_t sys_fork(void);
int32_t sys_exec(const char *filename, char *const argv[]);
int32_t sys_waitpid(pid_t pid);
uint32_t sys_sleep(uint32_t seconds);
void sys_exit(int status);

int sys_open(const char *filename);
int sys_lseek(int fd, int offset, int whence);
int sys_close(int fd);
int sys_dup(int oldfd);
int sys_dup2(int oldfd, int newfd);
int sys_pipe(int pipefd[2]);
int sys_read(int fd, uint8_t *buf, int len);
int sys_write(int fd, uint8_t *buf, int len);

int sys_chdir(const char *path);
int sys_lsdir(const char *path, uint8_t *buf);
int sys_mkdir(const char *path);
int sys_rmdir(const char *path);
int sys_unlink(const char *path);
int sys_stat(const char *path, uint8_t *buf);

uint32_t do_syscall(int id, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    switch (id) {
    case SYS_PRINTK:
        printk((char**)arg1);
        break;
    case SYS_GETPID:
        return current->pid;
        break;
    case SYS_FORK:
        return sys_fork();
        break;
    case SYS_EXEC:
        return sys_exec((char *)arg1, (char *const *)arg2);
        break;
    case SYS_EXIT:
        sys_exit(arg1);
        break;
    case SYS_WAITPID:
        return sys_waitpid(arg1);
        break;
    case SYS_SLEEP:
        return sys_sleep(arg1);
        break;
    case SYS_OPEN:
        break;
    case SYS_LSEEK:
        break;
    case SYS_CLOSE:
        break;
    case SYS_DUP:
        break;
    case SYS_DUP2:
        break;
    case SYS_PIPE:
        break;
    case SYS_READ:
        return sys_read(arg1, (uint8_t *)arg2, arg3);
        break;
    case SYS_WRITE:
        return sys_write(arg1, (uint8_t *)arg2, arg3);
        break;
    case SYS_CHDIR:
        return sys_chdir((char *)arg1);
        break;
    case SYS_LSDIR:
        return sys_lsdir((char *)arg1, (uint8_t *)arg2);
    case SYS_MKDIR:
        return sys_mkdir((char *)arg1);
    case SYS_RMDIR:
        return sys_rmdir((char *)arg1);
    case SYS_UNLINK:
        return sys_unlink((char *)arg1);
    case SYS_STAT:
        return sys_stat((char *)arg1, (uint8_t *)arg2);
    default:
        printf("Undefined system call!\n");
        return -1;
    }

    return 0;
}
