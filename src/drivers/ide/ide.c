#include "kernel.h"
#include "drivers/hal.h"
#include "drivers/ide.h"
#include "drivers/time.h"

#define WRITEBACK_TIME  1  /* writeback cache for every 1 second */

pid_t IDE;

static void ide_intr(void);
static void time_intr(void);
static void ide_driver_thread(void);

void cache_init(void);
void cache_writeback(void);
uint8_t read_byte(uint32_t);
void write_byte(uint32_t, uint8_t);

void
init_ide(void) {
    cache_init();
    add_irq_handle(14, ide_intr);
    add_irq_handle(0 , time_intr);
    Thread *p = create_kthread(ide_driver_thread);
    IDE = p->pid;
    hal_register("hda", IDE, 0);
    wakeup(p);
}

static void
ide_driver_thread(void) {
    static Message m;
    DevMessage *msg = (DevMessage *)&m;

    while (true) {
        receive(ANY, &m);

        if (m.src == MSG_HWINTR) {
            if (m.type == IDE_WRITEBACK) {
                cache_writeback();
            } else {
                panic("IDE interrupt is leaking");
            }
        } else if (m.type == MSG_DEVRD) {
            uint32_t i;
            uint8_t data;
            for (i = 0; i < msg->len; i ++) {
                data = read_byte(msg->offset + i);
                copy_from_kernel(find_tcb_by_pid(msg->req_pid), msg->buf + i, &data, 1);
            }
            msg->ret = i;
            m.dest = m.src;
            send(m.dest, &m);
        } else if (m.type == MSG_DEVWR) {
            uint32_t i;
            uint8_t data;
            for (i = 0; i < msg->len; i ++) {
                copy_to_kernel(find_tcb_by_pid(msg->req_pid), &data, msg->buf + i, 1);
                write_byte(msg->offset + i, data);
            }
            msg->ret = i;
            m.dest = m.src;
            send(m.dest, &m);
        }
        else {
            assert(0);
        }
    }
}

static void
ide_intr(void) {
    static Message m;
    m.type = IDE_READY;
    send(IDE, &m);
}
static void
time_intr(void) {
    static Message m;
    static uint32_t counter = 0;
    counter = (counter + 1) % (WRITEBACK_TIME * HZ);
    if (counter == 0) {
        m.type = IDE_WRITEBACK;
        send(IDE, &m);
    }
}
