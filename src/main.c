#include "common.h"
#include "x86.h"
#include "device.h"
#include "kernel.h"
#include "stdio.h"

// 用来测试的内核线程
void A()
{
	int cnt = 0;
	while (1) {
		cnt++;
		cnt %= 5000;
		if (cnt == 0) {
			printf("a");
		}
	}
}

void B()
{
	int cnt = 0;
	while (1) {
		cnt++;
		cnt %= 5000;
		if (cnt == 0) {
			printf("b");
		}
	}
}

void D()
{
	int cnt = 0;
	while (1) {
		cnt++;
		cnt %= 5000;
		if (cnt == 0) {
			printf("d");
		}
	}
}

void C()
{
	int i;

	for (i = 0; i < 5; ++i) {
		printf("c");
		create_kthread(D);
	}

	printf("\nthread will exit.\n");
}


void
entry(void) {
	init_timer();
	init_idt();
	init_intr();
	init_serial();

	init_threads();

	create_kthread(A);
	create_kthread(B);
	create_kthread(C);

	enable_interrupt();
	while (1) {
		wait_for_interrupt();
	}
	assert(0);
}


