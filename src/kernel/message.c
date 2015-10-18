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

void send(pid_t dst, Message *m)
{
    Thread *thread = find_tcb_by_pid(dst);
    if (!thread) {
        assert(0);
        return;
    }

    m->dest = dst;
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
        uint32_t i = 0;

        lock();

        for (i = current->msg_head; i != current->msg_tail; i = (i + 1) % NR_MSGS) {
            Message *msg = &current->msgs[i];
            if (msg->src == src || src == ANY) {
                *m = *msg; // 复制消息
                if (i != current->msg_head) {
                    // 保证消息按顺序接收
                    uint32_t j = i;
                    do {
                        int t = (j - 1 + NR_MSGS) % NR_MSGS;

                        current->msgs[j] = current->msgs[t];
                        j = t;
                    } while (j != current->msg_head);
                }
                current->msg_head++;
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
