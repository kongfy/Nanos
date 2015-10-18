#ifndef __SYSCALL_H__
#define __SYSCALL_H__

// User application's entry of Nanos kernel
int syscall(int id, ...);

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
#define SYS_MKDIR  20
#define SYS_RMDIR  21
#define SYS_UNLINK 22
#define SYS_STAT   23

#endif /* __SYSCALL_H__ */
