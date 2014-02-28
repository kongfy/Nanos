#include "common.h"
#include "x86.h"
#include "device.h"

void play(void);

/* 主循环，bootloader载入game二进制文件后，就跳转到这里执行 */
void
game_init(void) {
	init_timer();
	init_idt();
	init_intr();
	enable_interrupt();
	play(); /* 这里是演示代码，显示浮动的文字和键盘扫描码 */
	assert(0); /* 此处永远不应该返回，为什么？ */
}


#define PI 3.1415926
static int last_key = 0;
static volatile int timers = 0, live = 0;

/* 这是键盘中断处理程序。
   每当键盘中断进入系统后，do_irq.S和irq_handle.c中的代码会调用这个函数。
   code是按键的扫描码。
   中断处理是原子的(不会用重入中断)。 */
void
handle_keyboard(int code) {
	last_key = code;
	live = 20;
}

/* 这是时钟中断处理程序。
   每当时钟中断进入系统后，do_irq.S和irq_handle.c中的代码会调用这个函数。 */
void
handle_timer(void) { // 时钟中断处理函数
	timers ++;
}

float sin(float x) {
	float sign = 1;
	while (x > PI) x -= 2 * PI;
	while (x < -PI) x += 2 * PI;
	if (x < -PI / 2) { sign = -1; x += PI; }
	if (x > PI / 2) { sign = -1;  x -= PI; }
	float x2 = x * x;
	float x3 = x * x2;
	float x5 = x3 * x2;
	float x7 = x5 * x2;
	return sign * (x - x3 / 5.0 + x5 / 120.0 - x7 / 5040.0);
}

void
play(void) { //主循环
	const char *text = "http://cslab.nju.edu.cn/opsystem";
	static char buf[2];
	int text_len = 32;
	int w = SCR_WIDTH / 8;
	int color = 0, start_pos = 0, clk = 0;

	for (; ; clk = (clk + 1) % 5) { // 主循环是一个死循环
		int i;
		prepare_buffer(); // 在绘图之前，先需要准备缓冲区

		if (clk == 0) {
			start_pos = (start_pos + 1) % w;
		}
		color = (color + 1) % 72;
		for (i = 0; i < text_len; i ++) { // 计算每个字符的位置 然后显示在屏幕上
			int j = (i + start_pos) % w;
			int d = 50 * sin(2 * PI * j / w);
			buf[0] = text[i];
			draw_string(buf, 8 * j, SCR_HEIGHT / 2 - 4 + d, 32 + color);
		}
		
		// 在左下角显示键盘扫描码
		draw_string(itoa(last_key), 0, SCR_HEIGHT - 8, live > 0 ? 10: 7);
		if (live > 0) live --;

		i = HZ / 60;	
		while (i) {
			wait_for_interrupt();
			disable_interrupt(); // 关闭中断是为了防止数据竞争(data race)。
			if (timers > 0) {
				timers --;
				i --;
			}
			enable_interrupt();
		}

		display_buffer(); // 绘图结束后，调用这个函数将绘制的图像显示到屏幕上
	}
}
