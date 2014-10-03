#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "common.h"

uint32_t do_syscall(int id, uint32_t arg1, uint32_t arg2, uint32_t arg3);

// use kernel's printf function, debug only!
#define SYS_PRINTK 0
// get process pid
#define SYS_GETPID 1

#endif /* __SYSCALL_H__ */
