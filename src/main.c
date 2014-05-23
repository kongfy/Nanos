#include "common.h"
#include "x86.h"
#include "device.h"
#include "kernel.h"
#include "stdio.h"

#include "test.h"

void
entry(void) {
    init_timer();
    init_idt();
    init_intr();
    init_serial();

    init_threads();

    test_sleep_wakeup();

    enable_interrupt();
    while (1) {
        wait_for_interrupt();
    }
    assert(0);
}


