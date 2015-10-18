#include "kernel.h"
#include "fsys.h"
#include "hal.h"
#include "fsys_util.h"

pid_t FSYSD;
extern Device *fsys_dev;

void fsysd()
{
    assert(sizeof(FSYSMessage) <= sizeof(Message)); // Message结构体不能定义得太小

    Message m;
    fsys_dev = hal_get(FSYSDEV);

    while (1) {
        receive(ANY, &m);

        assert(m.src == FM);         // FM is the only thread can communicate with fsysd
        if (m.src == MSG_HWINTR) {
        } else {
            FSYSMessage *msg = (FSYSMessage *)&m;
            Thread *thread = find_tcb_by_pid(msg->req_pid);

            switch (m.type) {
            case MSG_FSYS_READ_BY_FILENAME: {
                iNode pwd = load_inode(msg->pwd);
                msg->ret = fsys_read_by_path(msg->filename, msg->buf, msg->offset, msg->len, thread, &pwd);
                send(m.src, &m);
                break;
            }
            case MSG_FSYS_INODE_FOR_FILENAME: {
                iNode pwd = load_inode(msg->pwd);
                iNode inode = fsys_path_to_inode(msg->filename, &pwd);
                msg->ret = 0;
                *(msg->inode) = inode;
                if (inode.index < 0) {
                    // not found
                    msg->ret = FILENOTFOUND;
                }
                send(m.src, &m);
                break;
            }
            case MSG_FSYS_CHDIR: {
                msg->ret = fsys_chdir(msg->filename, thread);
                send(m.src, &m);
                break;
            }
            case MSG_FSYS_LSDIR: {
                msg->ret = fsys_lsdir(msg->filename, msg->buf, thread);
                send(m.src, &m);
                break;
            }
            case MSG_FSYS_MKDIR: {
                msg->ret = fsys_mkdir(msg->filename, thread);
                send(m.src, &m);
                break;
            }
            case MSG_FSYS_RMDIR: {
                msg->ret = fsys_rmdir(msg->filename, thread);
                send(m.src, &m);
                break;
            }
            case MSG_FSYS_UNLINK: {
                msg->ret = fsys_unlink(msg->filename, thread);
                send(m.src, &m);
                break;
            }
            case MSG_FSYS_STAT: {
                msg->ret = fsys_stat(msg->filename, (struct stat*)msg->buf, thread);
                send(m.src, &m);
                break;
            }
            default:
                assert(0);
            }
        }
    }

    assert(0);
}
