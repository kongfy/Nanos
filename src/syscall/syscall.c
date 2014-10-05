#include "syscall.h"
#include "kernel.h"

int printk(char **formatp);
pid_t fork(void);

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
        return fork();
        break;
    case SYS_EXEC:
        break;
    default:
        panic("Undefined system call!\n");
        break;
    }

    return 0;
}
