/*
 * schedule.c
 *
 *  Created on: 2014-5-8
 *      Author: kongfy
 */

#include "kernel/schedule.h"
#include "kernel/kthread.h"

#include "memory.h"

bool need_sched = FALSE;

// 调度函数
void schedule(void)
{
    assert(need_sched);
    need_sched = FALSE;

    Thread *prev = current;
    Thread *next = NULL;

    if (list_empty(&queue.ready_queue)) {
        // 就绪队列空，取idle线程
        next = idle;
    } else {
        // 就绪队列非空
        if (prev == idle) {
            // 当前运行idle线程，从就绪队列中取队首线程调度
            next = list_entry(queue.ready_queue.next, Thread, runq);
            prev->status = Ready;
        } else {
            if (prev->status != Running) {
                // 当前运行线程退出或阻塞
                next = list_entry(queue.ready_queue.next, Thread, runq);
            } else {
                // 调度就绪队列中下一线程
                list_head *next_head = prev->runq.next;
                if (next_head == &queue.ready_queue) {
                    next_head = next_head->next;
                }

                next = list_entry(next_head, Thread, runq);
                prev->status = Ready;
            }
        }
    }

    next->status = Running;
    current = next;

    if (current->mm_struct) {
        CR3 cr3;
        cr3.val = 0;
        cr3.page_directory_base = ((uint32_t)current->mm_struct->pgd) >> 12;

        write_cr3(&cr3);
    } else {
        // kernel thread
        write_cr3(get_kcr3());
    }

    set_tss_esp0((uint32_t)&current->kstack[STK_SZ]);
}
