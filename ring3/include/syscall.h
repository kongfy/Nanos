#ifndef __SYSCALL_H__
#define __SYSCALL_H__

// User application's entry of Nanos kernel
int syscall(int id, ...);

// use kernel's printf function, debug only!
#define SYS_PRINTK 0

#endif /* __SYSCALL_H__ */
