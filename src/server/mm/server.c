/*
 * server.c
 *
 *  Created on: 2014-7-8
 *      Author: kongfy
 */

#include "kernel.h"
#include "common.h"

#include "server/mm.h"
#include "memory.h"

// MM服务器线程
void mm_server_thread()
{
    Message m;

    while (1) {
        receive(ANY, &m);

        if (m.src == MSG_HWINTR) {
            // by far, mm do not handle any bottom half
            assert(0);
        } else {
            MMMessage *msg = (MMMessage *)&m;
            Thread *thread = find_tcb_by_pid(msg->pid);

            switch (m.type) {
            case MSG_MM_CREATE_VM: {
                allocate_mm(thread);
                send(m.src, &m);
                break;
            }
            case MSG_MM_MMAP: {
                uint32_t vaddr = msg->vaddr;
                uint32_t len = msg->len;
                uint32_t read_write = msg->read_write;

                do_mmap(thread, vaddr, len, read_write);

                send(m.src, &m);
                break;
            }
            case MSG_MM_FORK: {
                Thread *child = find_tcb_by_pid(msg->child_pid);

                allocate_mm(child);
                copy_mm(thread, child);

                send(m.src, &m);
                break;
            }
            case MSG_MM_REVOKE_VM: {
                exit_mm(thread);
                send(m.src, &m);
                break;
            }
            }
        }
    }

    assert(0);
}
