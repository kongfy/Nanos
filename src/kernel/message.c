/*
 * message.c
 *
 *  Created on: 2014-5-23
 *      Author: kongfy
 */

#include "kernel/message.h"
#include "kernel.h"
#include "common.h"

bool is_messages_empty(Thread *thread)
{
    return thread->msg_head == thread->msg_tail;
}

void send(pid_t dst, Message *m)
{
    Thread *thread = find_tcb_by_pid(dst);
    if (!thread) {
        return;
    }

    m->type = 0; // 类型先随便填一个
    m->src = current->pid; // 怎么判断是否在中断中send?

    P(&thread->msg_mutex_lock);

    thread->msgs[thread->msg_tail++] = *m;
    thread->msg_tail %= NR_MSGS;

    // 信箱满了
    assert(thread->msg_head != thread->msg_tail);

    V(&thread->msg_mutex_lock);
    V(&thread->msg_sem);
}

void receive(pid_t src, Message *m)
{
    // TODO 没有检查src
    if (TRUE) {
        bool flag = FALSE;
        while (TRUE) {
            P(&current->msg_mutex_lock);

            if (!is_messages_empty(current)) {
                *m = current->msgs[current->msg_head++];
                current->msg_head %= NR_MSGS;
                flag = TRUE;
            }

            V(&current->msg_mutex_lock);

            if (flag) {
                break;
            }
            P(&current->msg_sem);
        }
    }
}
