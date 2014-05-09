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
    if (list_empty(&queue.ready_queue)) {
    	current = idle;
    } else {
    	if (current == idle) {
        	current = list_entry(queue.ready_queue.next, Thread, runq);
        } else {
    		list_head *next = current->runq.next;
    		if (next == &queue.ready_queue) {
    			next = next->next;
    		}

			current = list_entry(next, Thread, runq);
    	}
    }

}
