/*
 * schedule.c
 *
 *  Created on: 2014-5-8
 *      Author: kongfy
 */

#include "kernel/schedule.h"
#include "kernel/kthread.h"

#include "stdio.h"

// 调度函数
inline
void schedule(void)
{
	Thread *prev = current;
	Thread *next = NULL;

    if (list_empty(&queue.ready_queue)) {
    	next = idle;
    } else {
    	if (prev == idle || prev->status == Exit) {
        	next = list_entry(queue.ready_queue.next, Thread, runq);
        } else {
    		list_head *next_head = prev->runq.next;
    		if (next_head == &queue.ready_queue) {
    			next_head = next_head->next;
    		}

			next = list_entry(next_head, Thread, runq);
    	}
    }

    prev->status = Ready;
    next->status = Running;
    current = next;
}
