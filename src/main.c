#include "common.h"
#include "x86.h"
#include "device.h"
#include "kernel.h"

// 用来测试的内核线程
void A() { while (1) putchar('a'); }
void B() { while (1) putchar('b'); }

void
entry(void) {
	init_timer();
	init_idt();
	init_intr();
	init_serial();

	init_pcbs();

	create_kthread(A);
	create_kthread(B);

	enable_interrupt();
	while (1) {
		wait_for_interrupt();
	}
	assert(0);
}


