#include "kernel.h"
#include "drivers/tty.h"

pid_t TTY;

void init_tty(void);
void ttyd(void);
void send_keymsg(void);

void init_console();

void init_tty(void) {
    add_irq_handle(1, send_keymsg); // 在irq1时，调用send_keymsg函数
    init_console();
    TTY = create_kthread(ttyd)->pid;
}

