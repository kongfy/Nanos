#include "kernel.h"
#include "hal.h"
#include "drivers/tty.h"

void
ttyd(void) {
    Message m;

    while (1) {
        receive(ANY, &m);
        if (m.src == MSG_HWINTR) {
            // 中断下半段
            switch (m.type) {
                case MSG_TTY_GETKEY:
                    readkey();
                    break;
                case MSG_TTY_UPDATE:
                    update_banner();
                    break;
            }
        } else {
            DevMessage *msg;
            switch(m.type) {
                case MSG_DEVRD:
                    read_request((DevMessage*)&m);
                    break;
                case MSG_DEVWR:
                    msg = (DevMessage*)&m;
                    if (msg->dev_id >= 0 && msg->dev_id < NR_TTY) {
                        char c;
                        int i;
                        for (i = 0; i < msg->len; i ++) {
                            copy_to_kernel(find_tcb_by_pid(msg->req_pid), &c, (char*)msg->buf + i, 1);
                            consl_writec(&ttys[msg->dev_id], c);
                        }
                        consl_sync(&ttys[msg->dev_id]);
                    }
                    msg->ret = msg->len;
                    send(m.src, &m);
                    break;
            }
        }
    }
}

