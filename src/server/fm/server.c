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

#define MAX_REQ 256

typedef struct Request
{
    // key
    pid_t pid;
    int dev_id;
    pid_t req_pid;
    // valuep
    FMMessage msg;
} Request;

static Request buffer[MAX_REQ];
static uint32_t map[MAX_REQ / 32];

static inline
Request* get_free_buffer()
{
    int index = 0;
    for (index = 0; index < MAX_REQ; ++index) {
        uint32_t mask = 1U << (index % 32);
        if (!(map[index >> 5] & mask)) {
            map[index >> 5] |= mask;
            return &buffer[index];
        }
    }

    assert(0);
    return NULL;
}

static
void cache_request(Device* dev, pid_t req_pid, FMMessage *msg)
{
    Request *request = get_free_buffer();
    request->pid = dev->pid;
    request->dev_id = dev->dev_id;
    request->req_pid = req_pid;
    request->msg = *msg;
}

static
void reply(pid_t pid, int dev_id, pid_t req_pid, uint32_t ret)
{
    int index = 0;
    for (index = 0; index < MAX_REQ; ++index) {
        uint32_t mask = 1U << (index % 32);
        if (map[index >> 5] & mask) {
            if (buffer[index].pid == pid &&
                buffer[index].dev_id == dev_id &&
                buffer[index].req_pid == req_pid)
            {
                Message m;
                FMMessage *msg = (FMMessage *)&m;

                *msg = buffer[index].msg;
                msg->ret = ret;

                send(m.src, &m);
                map[index >> 5] &= ~mask;
                return;
            }
        }
    }

    assert(0);
}

// FM服务器线程
void fm_server_thread()
{
    Message m;

    while (1) {
        receive(ANY, &m);

        if (m.src == MSG_HWINTR) {
        } else {
            if (m.type == MSG_DEVRD || m.type == MSG_DEVWR) {
                // Device ACK
                DevMessage *dev_msg = (DevMessage *)&m;
                reply(m.src, dev_msg->dev_id, dev_msg->req_pid, dev_msg->ret);
                continue;
            }

            FMMessage *msg = (FMMessage *)&m;
            Thread *thread = find_tcb_by_pid(msg->req_pid);

            switch (m.type) {
            case MSG_FM_RD: {
                Device* dev = ram_read(msg->file_name, msg->dest_addr, msg->offset, msg->len, thread);
                cache_request(dev, thread->pid, msg);
                break;
            }
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
                Device* dev = do_read(thread, msg->fd1, (uint8_t *)msg->buf, msg->len);
                cache_request(dev, thread->pid, msg);
                break;
            }
            case MSG_FM_WRITE: {
                Device* dev = do_write(thread, msg->fd1, (uint8_t *)msg->buf, msg->len);
                cache_request(dev, thread->pid, msg);
                break;
            }
            default: {
                assert(0);
            }
            }
        }
    }

    assert(0);
}

