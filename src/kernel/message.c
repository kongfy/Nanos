/*
 * message.c
 *
 *  Created on: 2014-5-23
 *      Author: kongfy
 */

#include "kernel/message.h"
#include "kernel.h"
#include "common.h"

extern bool is_hwintr;

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
    if (is_hwintr) {
        m->src = MSG_HWINTR;
    } else {
        m->src = current->pid;
    }

    // 保护信箱临界区
    lock();

    thread->msgs[thread->msg_tail++] = *m;
    thread->msg_tail %= NR_MSGS;

    // 信箱满了
    assert(thread->msg_head != thread->msg_tail);

    // 如果有阻塞的receive，唤醒
    if (thread->msg_sem.count < 0) {
        assert(thread->msg_sem.count == -1); // 只能有一个receive被阻塞
        V(&thread->msg_sem);
    }

    unlock();
}

void receive(pid_t src, Message *m)
{
    bool flag = FALSE;
    while (TRUE) {
        int i = 0;

        lock();

        for (i = current->msg_head; i != current->msg_tail; i = (i + 1) % NR_MSGS) {
            Message *msg = &current->msgs[i];
            if (msg->src == src || src == ANY) {
                *m = current->msgs[current->msg_head++];
                current->msg_head %= NR_MSGS;
                flag = TRUE;
                break;
            }
        }

        if (flag) {
            // 接收消息成功
            unlock();
            break;
        } else {
            // 失败，阻塞
            assert(current->msg_sem.count == 0); // 这里必须被阻塞
            P(&current->msg_sem);
        }

        unlock();
    }
}
