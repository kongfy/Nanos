/*
 * ramdiskd.c
 *
 *  Created on: 2014-7-5
 *      Author: kongfy
 */

#include "kernel.h"
#include "hal.h"
#include "drivers/ramdisk.h"
#include "disk.h"

// static uint8_t *disk = (void*)buff;

// zero, 读取该设备会返回指定长度的'\0'字符(即值为0的字节), 写入该设备时数据将会被丢弃
static inline
void zero_handler(Message m)
{
    DevMessage *msg = (DevMessage *)&m;

    switch (m.type) {
        case MSG_DEVRD: {
            uint8_t zero = 0;
            int i;

            for (i = 0; i < msg->len; ++i) {
                copy_from_kernel(find_tcb_by_pid(msg->req_pid), msg->buf + i, &zero, 1);
            }

            msg->ret = i;
            send(m.src, &m);
            break;
        }
        case MSG_DEVWR:
            msg->ret = msg->len;
            send(m.src, &m);
            break;
    }
}

// null, 读取该设备总是马上返回(读取字节数总是为0), 写入该设备时数据将会被丢弃
static inline
void null_handler(Message m)
{
    DevMessage *msg = (DevMessage *)&m;

    switch (m.type) {
        case MSG_DEVRD:
            msg->ret = 0;
            send(m.src, &m);
            break;
        case MSG_DEVWR:
            msg->ret = msg->len;
            send(m.src, &m);
            break;
    }
}

static inline
void ramdisk_handler(Message m)
{
    DevMessage *msg = (DevMessage *)&m;

    switch (m.type) {
        case MSG_DEVRD: {
            int i;
            off_t offset = msg->offset;

            for (i = 0; i < msg->len && offset < NR_DISK_SIZE; ++i) {
                copy_from_kernel(find_tcb_by_pid(msg->req_pid), msg->buf + i, &disk[offset], 1);
                offset++;
            }

            msg->ret = i;
            send(m.src, &m);
            break;
        }
        case MSG_DEVWR: {
            int i;
            off_t offset = msg->offset;

            for (i = 0; i < msg->len && offset < NR_DISK_SIZE; ++i) {
                copy_to_kernel(find_tcb_by_pid(msg->req_pid), &disk[offset], msg->buf + i, 1);
                offset++;
            }

            msg->ret = i;
            send(m.src, &m);
            break;
        }
    }
}

void ramdiskd(void)
{
    Message m;

    while (1) {
        receive(ANY, &m);

        if (m.src == MSG_HWINTR) {
            // RAMDISK没有中断下半段
        } else {
            DevMessage *msg = (DevMessage *)&m;

            switch (msg->dev_id) {
                case RAMDISK_ID:
                    ramdisk_handler(m);
                    break;
                case NULL_ID:
                    null_handler(m);
                    break;
                case ZERO_ID:
                    zero_handler(m);
                    break;
                default:
                    panic("Unknown ramdisk dev_id!\n");
            }
        }
    }
}

