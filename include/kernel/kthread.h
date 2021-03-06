/*
 * kthread.h
 *
 *  Created on: 2014-5-8
 *      Author: kongfy
 */

#ifndef __KERNEL_KTHREAD_H__
#define __KERNEL_KTHREAD_H__

#include "kernel/list.h"
#include "kernel/sem.h"
#include "kernel/message.h"
#include "x86.h"
#include "memory/mm_types.h"
#include "fsys/fs_types.h"

#define INTR assert(read_eflags() & IF_MASK)
#define NOINTR assert(~read_eflags() & IF_MASK)

#define STK_SZ 4096
#define NR_MSGS 64

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
    uint32_t if_status;
    uint32_t lock_count;

    // 消息信箱和信号量
    Semaphore msg_sem;
    volatile uint32_t msg_head, msg_tail;
    Message msgs[NR_MSGS];

    list_head runq, semq;

    // 进程内存管理结构，内核进程为NULL
    mm_struct *mm_struct;
    // file management
    fm_struct *fm_struct;

    char kstack[STK_SZ];
} Thread;

typedef struct TCBQueue
{
    list_head ready_queue, wait_queue;
} TCBQueue;

extern Thread *current;
extern Thread *idle;
extern TCBQueue queue;

// 根据pid返回对应的pcb
Thread *find_tcb_by_pid(pid_t pid);

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

/* <========================= 给用户进程的数据接口，提供给PM使用 ==========================> */

// 获取一个PCB结构，供PM使用，必须自行负责销毁
Thread *create_thread();
inline void thread_exit(Thread *Thread);
inline void thread_ready(Thread *thread); // 使进程进入就绪队列

#endif /* __KERNEL_KTHREAD_H__ */
