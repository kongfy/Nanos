#include "kernel.h"
#include "drivers/tty.h"
#include "drivers/hal.h"

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
                        int i;
                        for (i = 0; i < msg->len; i ++) {
                            consl_writec(&ttys[msg->dev_id], ((char*)msg->buf)[i]);
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

