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

    assert(sizeof(FMMessage) <= sizeof(Message)); // Message结构体不能定义得太小

    while (1) {
        receive(ANY, &m);

        if (m.src == MSG_HWINTR) {
        } else {
            if (m.type == MSG_DEVRD || m.type == MSG_DEVWR) {
                // Device ACK
                DevMessage *dev_msg = (DevMessage *)&m;
                Request_key key;
                key.type = REQ_DEV;
                key.key.dev.req_pid = dev_msg->req_pid;
                key.key.dev.pid = m.src;
                key.key.dev.dev_id = dev_msg->dev_id;

                reply(key, dev_msg->ret);
                continue;
            }

            if (m.src == FSYSD) {
                // fsys ACK
                FSYSMessage *fsys_msg = (FSYSMessage *)&m;
                Request_key key;
                key.type = REQ_FSYS;
                key.key.fsys.req_pid = fsys_msg->req_pid;

                reply(key, fsys_msg->ret);
                continue;
            }

            FMMessage *msg = (FMMessage *)&m;
            Thread *thread = find_tcb_by_pid(msg->req_pid);

            switch (m.type) {
            case MSG_FM_RD: {
                // ONLY used by pm to read off ELF header from disk
                Thread *user = msg->usr_pid ? find_tcb_by_pid(msg->usr_pid) : NULL;
                Request_key key = fs_read(msg->filename, msg->dest_addr, msg->offset, msg->len, thread, user);
                cache_request(key, msg);
                break;
            }
            case MSG_FM_WR:
                // better not be implement
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
                Request_key key = do_read(thread, msg->fd1, (uint8_t *)msg->buf, msg->len);
                cache_request(key, msg);
                break;
            }
            case MSG_FM_WRITE: {
                Request_key key = do_write(thread, msg->fd1, (uint8_t *)msg->buf, msg->len);
                cache_request(key, msg);
                break;
            }
            case MSG_FM_CHDIR: {
                Request_key key = do_chdir(thread, msg->filename);
                cache_request(key, msg);
                break;
            }
            case MSG_FM_LSDIR: {
                Request_key key = do_lsdir(thread, msg->filename, (uint8_t *)msg->buf);
                cache_request(key, msg);
                break;
            }
            case MSG_FM_MKDIR: {
                Request_key key = do_mkdir(thread, msg->filename);
                cache_request(key, msg);
                break;
            }
            case MSG_FM_RMDIR: {
                Request_key key = do_rmdir(thread, msg->filename);
                cache_request(key, msg);
                break;
            }
            case MSG_FM_UNLINK: {
                Request_key key = do_unlink(thread, msg->filename);
                cache_request(key, msg);
                break;
            }
            case MSG_FM_STAT: {
                Request_key key = do_stat(thread, msg->filename, (struct stat *)msg->buf);
                cache_request(key, msg);
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

