#include "common.h"
#include "x86.h"
#include "device.h"

// 用来测试的内核线程
void A() { while (1) putchar('a'); }
void B() { while (1) putchar('b'); }

void
entry(void) {
	init_timer();
	init_idt();
	init_intr();
	init_serial();
	enable_interrupt();
	while (1) {
		wait_for_interrupt();
	}
	assert(0);
}


