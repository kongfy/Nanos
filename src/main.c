#include "common.h"
#include "x86.h"
#include "device.h"
#include "kernel.h"
#include "stdio.h"

#include "test.h"

void keyboard_irq(void)
{
    uint32_t code = in_byte(0x60);
    uint32_t val = in_byte(0x61);
    out_byte(0x61, val | 0x80);
    out_byte(0x61, val);
    printf("%d\n", code);
}

void
entry(void) {
    init_timer();
    init_idt();
    init_intr();
    init_serial();

    add_irq_handle(1, keyboard_irq);

    init_threads();

    test_message();

    enable_interrupt();
    while (1) {
        wait_for_interrupt();
    }
    assert(0);
}
