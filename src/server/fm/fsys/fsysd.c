#include "kernel.h"
#include "fsys.h"
#include "hal.h"

pid_t FSYSD;
extern Device *fsys_dev;

size_t fsys_read_by_filename(const char *filename, uint8_t *buf, off_t offset, size_t len, Thread *thread);

void fsysd()
{
    assert(sizeof(FSYSMessage) <= sizeof(Message)); // Message结构体不能定义得太小

    Message m;
    fsys_dev = hal_get("ram");

    while (1) {
        receive(ANY, &m);

        assert(m.src == FM);         // FM is the only thread can communicate with fsysd
        if (m.src == MSG_HWINTR) {
        } else {
            FSYSMessage *msg = (FSYSMessage *)&m;
            Thread *thread = find_tcb_by_pid(msg->req_pid);

            switch (m.type) {
            case MSG_FSYS_READ_BY_FILENAME: {
                msg->ret = fsys_read_by_filename(msg->filename, msg->buf, msg->offset, msg->len, thread);
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
