#include "common.h"
#include "x86.h"
#include "device.h"

/* 8253输入频率为1.193182MHz */
// PIT输出Channel 0在x86的I/O ports中编号为0x40
#define TIMER_PORT 0x40
#define FREQ_8253 1193182

void
init_timer(void) {
	int counter = FREQ_8253 / HZ;
	// 寄存器（16位Reload register）最高不能超过65536
	assert(counter < 65536);

	// 将PIC的0x43 Mode/Command register写为: 00 11 010 0
	// Select channel : Channel 0
	// Access mode : lobyte/hibyte
	// Operating mode : Mode 2 (rate generator), 通常操作系统使用Mode 3(square wave generator)产生IRQ0时钟中断，但使用Mode 2可以得到更精确的时钟频率
	// BCD/Binary mode: 0 = 16-bit binary
	out_byte(TIMER_PORT + 3, 0x34);

	// 下面两个写操作写入PIT的Channel 0，设置counter
	// 因为Access mode为lobyte/hibyte，所以两次先后分别写入低8位和高8位
	out_byte(TIMER_PORT + 0, counter % 256);
	out_byte(TIMER_PORT + 0, counter / 256);
}

// 详细参见 http://wiki.osdev.org/Programmable_Interval_Timer
// 另外一个简洁的介绍 http://en.wikibooks.org/wiki/X86_Assembly/Programmable_Interval_Timer
