#include "kernel.h"
#include "drivers/tty.h"

pid_t TTY;

void init_tty(void);
void init_getty(void);
void ttyd(void);
void send_keymsg(void);

void init_console();

void init_tty(void) {
    add_irq_handle(1, send_keymsg); // 在irq1时，调用send_keymsg函数
    init_console();
    TTY = create_kthread(ttyd)->pid;

    int i;
    lock();
    for (i = 0; i < NR_TTY; i ++) {
        hal_register(ttynames[i], TTY, i);
    }
    unlock();

    init_getty();
}

