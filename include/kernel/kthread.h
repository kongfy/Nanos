/*
 * kthread.h
 *
 *  Created on: 2014-5-8
 *      Author: kongfy
 */

#ifndef __KERNEL_KTHREAD_H__
#define __KERNEL_KTHREAD_H__

#include "kernel/list.h"
#include "x86.h"

#define STK_SZ 4096

typedef enum Status
{
    Ready = 0,
    Running,
    Block,
    Exit,
} Status;

typedef struct Thread
{
    TrapFrame *tf;
    pid_t pid;
    Status status;
    uint32_t lock_count;

    list_head runq, semq;
    char kstack[STK_SZ];
} Thread;

typedef struct TCBQueue
{
    list_head ready_queue, wait_queue;
} TCBQueue;

extern Thread *current;
extern Thread *idle;
extern TCBQueue queue;

// 初始化，创建0号进程
void init_threads(void);
// 创建一个内核线程
Thread *create_kthread(void (*entry)(void));
// 使当前进程/线程立即阻塞，并可以在未来被唤醒
void sleep(void);
// 唤醒一个进程/线程
void wakeup(Thread *t);
// 短临界区保护，实现关中断保护的原子操作
void lock(void);
void unlock(void);

#endif /* __KERNEL_KTHREAD_H__ */
