/*
 * kthread.c
 *
 *  Created on: 2014-5-8
 *      Author: kongfy
 */


#include "kernel/kthread.h"
#include "kernel/list.h"
#include "kernel/pid.h"

#include "x86.h"
#include "stdio.h"

// 全局变量
Thread *current;
Thread *idle;
TCBQueue queue;

// 文件作用域
static Thread tcbs[MAX_PROCESS];

// 线程退出函数
void exit(void);

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
	current = idle;
}

// 创建一个内核线程
Thread *create_kthread(void (*entry)(void))
{
	pid_t pid = get_free_pid();
	if (pid < 0) {
		printf("too many threads!\n");
		return NULL;
	}

	Thread *thread = &tcbs[pid];
	thread->pid = pid;
	TrapFrame *tf = ((TrapFrame *)(thread->kstack + STK_SZ)) - 1;
	tf->eax = tf->ebx = tf->ecx = tf->edx = tf->esi = tf->edi = tf->ebp = 0;
	tf->cs = SELECTOR_KERNEL(SEG_KERNEL_CODE);
	tf->eip = (uint32_t)entry;
	tf->eflags = 1 << 9; // 置IF位
	tf->ds = tf->es = tf->ss = SELECTOR_KERNEL(SEG_KERNEL_DATA);
	thread->tf = tf;

	list_add_tail(&thread->runq, &queue.ready_queue);

	return thread;
}

// 使当前进程/线程立即阻塞，并可以在未来被唤醒
void sleep(void)
{
}

// 唤醒一个进程/线程
void wakeup(Thread *t)
{
}

// 短临界区保护，实现关中断保护的原子操作
void lock(void)
{
}

void unlock(void)
{
}

// 线程退出函数
void exit(void)
{

}

