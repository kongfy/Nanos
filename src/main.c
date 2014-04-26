#include "common.h"
#include "x86.h"
#include "device.h"

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


