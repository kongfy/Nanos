#include "common.h"
#include "x86.h"
#include "device.h"
#include "kernel.h"
#include "stdio.h"

// 用来测试的内核线程
Thread *tcb_a, *tcb_b, *tcb_c, *tcb_d;

void A () {
    int x = 0;

    while(1) {
        if(x % 100000 == 0) {
            printf("a");
            wakeup(tcb_b);
            sleep();
        }
        x ++;
    }
}
void B () {
    int x = 0;

    sleep();
    while(1) {
        if(x % 100000 == 0) {
            printf("b");
            wakeup(tcb_c);
            sleep();
        }
        x ++;
    }
}
void C () {
    int x = 0;

    sleep();
    while(1) {
        if(x % 100000 == 0) {
            printf("c");
            wakeup(tcb_d);
            sleep();
        }
        x ++;
    }
}
void D () {
    int x = 0;

    sleep();
    while(1) {
        if(x % 100000 == 0) {
            printf("d");
            wakeup(tcb_a);
            sleep();
        }
        x ++;
    }
}

void
entry(void) {
	init_timer();
	init_idt();
	init_intr();
	init_serial();

	init_threads();

	tcb_d = create_kthread(D);
	tcb_c = create_kthread(C);
	tcb_b = create_kthread(B);
	tcb_a = create_kthread(A);

	enable_interrupt();
	while (1) {
		wait_for_interrupt();
	}
	assert(0);
}


