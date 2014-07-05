#include "common.h"
#include "x86.h"
#include "device.h"
#include "drivers.h"
#include "server.h"
#include "kernel.h"
#include "stdio.h"
#include "memory.h"

#include "test.h"

void keyboard_irq(void)
{
    uint32_t code = in_byte(0x60);
    uint32_t val = in_byte(0x61);
    out_byte(0x61, val | 0x80);
    out_byte(0x61, val);
    printf("%d\n", code);
}

void init_kernel(void);

void
entry(void) {
    /* Notice that when we are here, IF is always 0 (see bootloader) */

    /* We must set up kernel virtual memory first because our kernel
       thinks it is located in 0xC0000000.
       Before setting up correct paging, no global variable can be used. */
    init_page();

    /* After paging is enabled, we can jump to the high address to keep
     * consistent with virtual memory, although it is not necessary. */
    asm volatile (" addl %0, %%esp\n\t\
                    jmp *%1": : "r"(KOFFSET), "r"(init_kernel));

    assert(0);  // should not reach here
}

void
init_kernel(void) {
    /* Reset the GDT. Although user processes in Nanos run in Ring 0,
       they have their own virtual address space. Therefore, the
       old GDT located in physical address 0x7C00 cannot be used again. */
    init_segment();
    init_idt();
    init_intr();

    // create idle thread
    init_threads();

    // device
    init_serial();

    // drivers
    //init_drivers();

    // servers
    //init_fm();

    test_ab_print();

    enable_interrupt();
    while (1) {
        wait_for_interrupt();
    }
    assert(0);
}
