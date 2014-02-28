#include "x86.h"

void handle_timer(void);
void handle_keyboard(int);

/* TrapFrame的定义在include/x86/memory.h
 * 请仔细理解这段程序的含义，这些内容将在后续的实验中被反复使用。 */
void
irq_handle(struct TrapFrame *tf) {
	if (tf->irq == 1000) {
		// 时钟中断在此处理
		handle_timer();
	} else if (tf->irq == 1001) {
		// 键盘中断在此处理
		uint32_t code = in_byte(0x60);
		uint32_t val = in_byte(0x61);
		out_byte(0x61, val | 0x80);
		out_byte(0x61, val);
		handle_keyboard(code);
	} else {
		// 其他类型中断/异常，通常是因为你的代码中包含错误
		assert(0);
	}
}

