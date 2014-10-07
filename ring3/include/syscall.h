#ifndef __SYSCALL_H__
#define __SYSCALL_H__

// User application's entry of Nanos kernel
int syscall(int id, ...);

// use kernel's printf function, debug only!
#define SYS_PRINTK 0
// get process pid
#define SYS_GETPID 1
// fork
#define SYS_FORK 2
// exec
#define SYS_EXEC 3
// exit
#define SYS_EXIT 4
// waitpid
#define SYS_WAITPID 5
// sleep
#define SYS_SLEEP 6

#endif /* __SYSCALL_H__ */
