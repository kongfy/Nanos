#include "syscall.h"
#include "kernel.h"

int printk(char **formatp);
pid_t sys_fork(void);
int sys_exec(int filename, char *const argv[]);
void sys_exit(int status);

uint32_t do_syscall(int id, uint32_t arg1, uint32_t arg2, uint32_t arb3)
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
        return sys_exec(arg1, (char *const *)arg2);
        break;
    case SYS_EXIT:
        sys_exit(arg1);
        break;
    default:
        panic("Undefined system call!\n");
        break;
    }

    return 0;
}
