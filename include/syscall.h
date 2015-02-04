#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "common.h"

uint32_t do_syscall(int id, uint32_t arg1, uint32_t arg2, uint32_t arg3);

#define SYS_PRINTK  0
#define SYS_GETPID  1
#define SYS_FORK    2
#define SYS_EXEC    3
#define SYS_EXIT    4
#define SYS_WAITPID 5
#define SYS_SLEEP   6
#define SYS_OPEN   10
#define SYS_LSEEK  11
#define SYS_CLOSE  12
#define SYS_DUP    13
#define SYS_DUP2   14
#define SYS_PIPE   15
#define SYS_READ   16
#define SYS_WRITE  17
#define SYS_CHDIR  18
#define SYS_LSDIR  19

#endif /* __SYSCALL_H__ */
