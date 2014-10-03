#include "syscall.h"

void do_syscall(int id, uint32_t arg1, uint32_t arg2, uint32_t arb3)
{
    switch (id) {
    case SYS_PRINTK:
        printf("printk");
    }
}
