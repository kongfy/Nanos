/*
 * server.c
 *
 *  Created on: 2014-7-5
 *      Author: kongfy
 */

#include "kernel.h"
#include "hal.h"

#include "drivers/ramdisk.h"
#include "server/fm.h"
#include "fsys.h"

// FM服务器线程
void fm_server_thread()
{
    Message m;

    while (1) {
        receive(ANY, &m);

        if (m.src == MSG_HWINTR) {
        } else {
            FMMessage *msg = (FMMessage *)&m;
            Thread *thread = find_tcb_by_pid(msg->req_pid);

            switch (m.type) {
            case MSG_FM_RD:
                msg->ret = ram_read(msg->file_name, msg->dest_addr, msg->offset, msg->len, thread);
                send(m.src, &m);
                break;
            case MSG_FM_WR:
                break;
            case MSG_FM_INIT: {
                init_fm_tty(thread, msg->tty);
                send(m.src, &m);
                break;
            }
            case MSG_FM_COPY: {
                Thread *child = find_tcb_by_pid(msg->child_pid);
                copy_fm(thread, child);
                send(m.src, &m);
                break;
            }
            case MSG_FM_EXIT: {
                exit_fm(thread);
                send(m.src, &m);
                break;
            }
            case MSG_FM_READ: {
                msg->ret = do_read(thread, msg->fd1, (uint8_t *)msg->buf, msg->len);
                send(m.src, &m);
                break;
            }
            case MSG_FM_WRITE: {
                msg->ret = do_write(thread, msg->fd1, (uint8_t *)msg->buf, msg->len);
                send(m.src, &m);
                break;
            }
            }
        }
    }

    assert(0);
}

