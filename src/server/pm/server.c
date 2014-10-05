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

void create_first_process();
pid_t fork_process(Thread *Thread);

// PM服务器线程
void pm_server_thread()
{
    create_first_process();

    Message m;

    while (1) {
        receive(ANY, &m);

        if (m.src == MSG_HWINTR) {
        } else {
            PMMessage *msg = (PMMessage *)&m;
            Thread *thread = find_tcb_by_pid(m.src);

            switch (m.type) {
                case MSG_PM_FORK: {
                    msg->ret = fork_process(thread);
                    send(m.src, &m);
                    break;
                }
            }
        }
    }

    assert(0);
}
