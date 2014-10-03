#include "syscall.h"
#include "kernel.h"

extern int printk(char **formatp);

uint32_t do_syscall(int id, uint32_t arg1, uint32_t arg2, uint32_t arb3)
{
    switch (id) {
    case SYS_PRINTK:
        printk((char**)arg1);
        break;
    case SYS_GETPID:
        return current->pid;
        break;
    }

    return 0;
}
