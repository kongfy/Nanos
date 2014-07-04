/*
 * kthread.c
 *
 *  Created on: 2014-5-8
 *      Author: kongfy
 */


#include "kernel/kthread.h"
#include "kernel/list.h"
#include "kernel/pid.h"
#include "kernel/sem.h"

#include "common.h"
#include "x86.h"
#include "stdio.h"

// 全局变量
Thread *current;
Thread *idle;
TCBQueue queue;

// 文件作用域
static Thread tcbs[MAX_PROCESS];

// 线程退出函数
void kthread_exit(void);

// 初始化，创建0号进程
void init_threads(void)
{
    INIT_LIST_HEAD(&queue.ready_queue);
    INIT_LIST_HEAD(&queue.wait_queue);

    init_pid();

    pid_t pid = get_free_pid();
    assert(pid == 0);
    // 0号进程使用内核默认栈

    idle = &tcbs[pid];
    idle->pid = pid;
    idle->status = Running;
    idle->lock_count = 0;
    
    current = idle;
}

// 创建一个内核线程
Thread *create_kthread(void (*entry)(void))
{
    lock();
    pid_t pid = get_free_pid();
    unlock();

    if (pid < 0) {
        printf("too many threads!\n");
        return NULL;
    }

    Thread *thread = &tcbs[pid];
    thread->pid = pid;
    thread->status = Ready;
    thread->lock_count = 0;

    uint32_t *exit_addr = (uint32_t *)(thread->kstack + STK_SZ) - 1;
    *exit_addr = (uint32_t)kthread_exit;

    TrapFrame *tf = (TrapFrame *)(exit_addr) - 1;
    tf->eax = tf->ebx = tf->ecx = tf->edx = tf->esi = tf->edi = tf->ebp = tf->esp_ = 0;
    tf->cs = SELECTOR_KERNEL(SEG_KERNEL_CODE);
    tf->eip = (uint32_t)entry;
    tf->eflags = 1 << 9; // 置IF位
    tf->err = tf->irq = 0;
    tf->gs = tf->fs = 0;
    tf->ds = tf->es = SELECTOR_KERNEL(SEG_KERNEL_DATA);
    thread->tf = tf;

    // 初始化消息队列信息
    init_sem(&thread->msg_sem, 0);
    thread->msg_head = thread->msg_tail = 0;

    lock();
    list_add_tail(&thread->runq, &queue.ready_queue);
    unlock();

    return thread;
}

// 短临界区保护，实现关中断保护的原子操作
void lock(void)
{
    if (current->lock_count == 0) {
        current->if_status = read_eflags() & IF_MASK;

        if (current->if_status) {
            INTR;
            disable_interrupt();
        }
    }

    NOINTR;
    current->lock_count++;
}

void unlock(void)
{
    NOINTR;
    assert(current->lock_count > 0);
    current->lock_count--;

    if (current->lock_count == 0) {
        if (current->if_status) {
            enable_interrupt();
            INTR;
        }
    }
}

// 使当前进程/线程立即阻塞，并可以在未来被唤醒
void sleep(void)
{
    lock();

    current->status = Block;
    list_del(&current->runq);
    list_add_tail(&current->runq, &queue.wait_queue);

    asm volatile("int $0x80");

    unlock();
}

// 唤醒一个进程/线程
void wakeup(Thread *t)
{
    if (t->status != Block) {
        return;
    }

    lock();

    t->status = Ready;
    list_del(&t->runq);
    list_add_tail(&t->runq, &queue.ready_queue);

    unlock();
}

// 内核线程退出函数
void kthread_exit(void)
{
    // 释放资源
    lock();

    list_del(&current->runq);
    free_pid(current->pid);
    current->status = Exit;

    asm volatile("int $0x80");

    unlock();
}

Thread *find_tcb_by_pid(pid_t pid)
{
    if (is_pid_available(pid)) {
        return NULL;
    }

    return &tcbs[pid];
}

