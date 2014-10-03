#include "x86.h"
#include "device.h"
#include "kernel.h"
#include "common.h"
#include "syscall.h"


#define NR_IRQ_HANDLE 32

/* In Nanos, there are no more than 16(actually, 3) hardward interrupts. */
#define NR_HARD_INTR 16

/* Structures below is a linked list of function pointers indicating the
   work to be done for each interrupt. Routines will be called one by
   another when interrupt comes.
   For example, the timer interrupts are used for alarm clocks, so
   device driver of timer will use add_irq_handle to attach a simple
   alarm checker. Also, the IDE driver needs to write back dirty cache
   slots periodically. So the IDE driver also calls add_irq_handle
   to register a handler. */

struct IRQ_t {
    void (*routine)(void);
    struct IRQ_t *next;
};

static struct IRQ_t handle_pool[NR_IRQ_HANDLE];
static struct IRQ_t *handles[NR_HARD_INTR];
static int handle_count = 0;

void
add_irq_handle(int irq, void (*func)(void) ) {
    struct IRQ_t *ptr;
    assert(irq < NR_HARD_INTR);
    if (handle_count > NR_IRQ_HANDLE) {
        panic("Too many irq registrations!");
    }
    ptr = &handle_pool[handle_count ++]; /* get a free handler */
    ptr->routine = func;
    ptr->next = handles[irq]; /* insert into the linked list */
    handles[irq] = ptr;
}

bool is_hwintr = FALSE;

void irq_handle(TrapFrame *tf) {
    int irq = tf->irq;

    if (irq < 0) {
        panic("Unhandled exception!");
    }

    if (irq < 1000) {
        if (irq == 0x80) {
            // judge by cs is a better choise, but not now!
            // if (SELECTOR_KERNEL(SEG_USER_CODE) == tf->cs) {
            if (Running == current->status) {
                // system call
                uint32_t ret = do_syscall(tf->eax,
                                          tf->ebx,
                                          tf->ecx,
                                          tf->edx);
                tf->eax = ret; // return value saved in reg eax;
            } else {
                // force schedule
                need_sched = TRUE;
            }
        } else {
            panic("Unexpected exception #%d\n\33[1;31mHint: The machine is always right! For more details about exception #%d, see\n%s\n\33[0m", irq, irq, logo_i386);
        }
    } else if (irq >= 1000) {
        int irq_id = irq - 1000;
        assert(irq_id < NR_HARD_INTR);
        struct IRQ_t *f = handles[irq_id];

        is_hwintr = TRUE;
        while (f != NULL) { /* call handlers one by one */
            f->routine();
            f = f->next;
        }
        is_hwintr = FALSE;
    }

    current->tf = tf;
    if (need_sched) {
        schedule();
    }
}
