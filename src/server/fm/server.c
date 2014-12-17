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

#define NR_BUFF 4096

static uint8_t fm_buf[NR_BUFF];

static size_t
do_read(int file_name, uint8_t *buf, off_t offset, size_t len)
{
    Device *dev = hal_get("ram");

    offset += file_name * NR_FILE_SIZE;
    return dev_read(dev, current->pid, offset, buf, len);
}

// FM服务器线程
void fm_server_thread()
{
    Message m;

    while (1) {
        receive(ANY, &m);

        if (m.src == MSG_HWINTR) {
        } else {
            FMMessage *msg = (FMMessage *)&m;

            switch (m.type) {
                case MSG_FM_RD:
                    assert(msg->len < NR_BUFF);

                    // 先和驱动交互拷贝到内核缓冲区，完成后拷贝至请求进程
                    msg->ret = do_read(msg->file_name, fm_buf, msg->offset, msg->len);
                    msg->ret = copy_from_kernel(find_tcb_by_pid(msg->req_pid), msg->dest_addr, fm_buf, msg->ret);
                    send(m.src, &m);
                    break;
                case MSG_FM_WR:
                    break;
            }
        }
    }

    assert(0);
}

