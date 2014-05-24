#include "x86.h"
#include "device.h"
#include "stdio.h"
#include "kernel.h"
#include "common.h"

bool is_hwintr = FALSE;

void irq_handle(struct TrapFrame *tf) {
    current->tf = tf;

    if (tf->irq == 1000) {
        is_hwintr = TRUE;
        schedule();
    } else if (tf->irq == 1001) {
        is_hwintr = TRUE;
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

    is_hwintr = FALSE;
}

