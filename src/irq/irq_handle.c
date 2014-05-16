#include "x86.h"
#include "device.h"
#include "stdio.h"
#include "kernel.h"

void irq_handle(struct TrapFrame *tf) {
    current->tf = tf;

    if (tf->irq == 1000) {
        schedule();
    } else if (tf->irq == 1001) {
        uint32_t code = in_byte(0x60);
        uint32_t val = in_byte(0x61);
        out_byte(0x61, val | 0x80);
        out_byte(0x61, val);
        printf("%d\n", code);
    } else if (tf->irq == 0x80) {
        schedule();
    } else {
        if (tf->irq == -1) {
            printf("\nUnhandled exception!\n");
        } else {
            printf("\nUnexpected exception #%d\n", tf->irq);
        }

        assert(0);
    }
}

