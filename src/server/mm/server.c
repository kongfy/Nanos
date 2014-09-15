/*
 * server.c
 *
 *  Created on: 2014-7-8
 *      Author: kongfy
 */

#include "kernel.h"

#include "server/mm.h"
#include "memory.h"

// MM服务器线程
void mm_server_thread()
{
    Message m;

    while (1) {
        receive(ANY, &m);

        if (m.src == MSG_HWINTR) {
        } else {
            MMMessage *msg = (MMMessage *)&m;
            Thread *thread = find_tcb_by_pid(msg->pid);

            switch (m.type) {
                case MSG_MM_CREATE_VM: {
                    create_vm(thread);
                    send(m.src, &m);
                    break;
                }
                case MSG_MM_ALLOC_PAGES: {
                    unsigned int vaddr = msg->vaddr;
                    unsigned int memsz = msg->memsz;

                    unsigned int paddr = alloc_pages(thread, vaddr, memsz);

                    msg->paddr = paddr;
                    send(m.src, &m);
                    break;
                }
            }
        }
    }

    assert(0);
}
