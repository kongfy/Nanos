/*
 * server.c
 *
 *  Created on: 2014-7-8
 *      Author: kongfy
 */

#include "kernel.h"
#include "server.h"

#include "hal.h"
#include "elf.h"

void create_shells();
pid_t do_fork(Thread *thread);
int do_exec(Thread *thread, const char *filename, char *argv[]);
void do_waitpid(Thread *thread, pid_t pid);
void do_exit(Thread *thread, int status);

// PM服务器线程
void pm_server_thread()
{
    create_shells();

    Message m;

    assert(sizeof(PMMessage) <= sizeof(Message)); // Message结构体不能定义得太小

    while (1) {
        receive(ANY, &m);

        if (m.src == MSG_HWINTR) {
        } else {
            PMMessage *msg = (PMMessage *)&m;
            Thread *thread = find_tcb_by_pid(m.src);

            switch (m.type) {
            case MSG_PM_FORK: {
                msg->ret = do_fork(thread);
                send(m.src, &m);
                break;
            }
            case MSG_PM_EXEC: {
                msg->ret = do_exec(thread, msg->filename, (char **)msg->argv);
                if (msg->ret < 0) {
                    send(m.src, &m);
                }
                break;
            }
            case MSG_PM_WAITPID: {
                do_waitpid(thread, msg->pid);
                // do not reply process until target process exit
                break;
            }
            case MSG_PM_EXIT: {
                do_exit(thread, msg->status);
                break;
            }
            case MSG_PM_KILL : {
                thread = find_tcb_by_pid(msg->pid);
                do_exit(thread, msg->status);
                if (m.src != msg->pid) {
                    send(m.src, &m);
                }
                break;
            }

            }
        }
    }

    assert(0);
}
