#include "kernel.h"
#include "common.h"
#include "string.h"

#include "drivers/time.h"
#include "drivers/hal.h"


pid_t TIME;

long jiffy = 0;
void init_timer(void);
void timerd(void);
static Time rt;

void init_i8253(void);
void update_jiffy(void);
int read_rtc(int);

void init_timer(void) {
    init_i8253();
    add_irq_handle(0, update_jiffy);

    TIME = create_kthread(timerd)->pid;
    hal_register("timer", TIME, 0);

    int tmp;
    do {
        rt.second = read_rtc(0);
        rt.minute = read_rtc(2);
        rt.hour = read_rtc(4);
        rt.day = read_rtc(7);
        rt.month = read_rtc(8);
        rt.year = read_rtc(9) + 2000;
        tmp = read_rtc(0);
    } while (tmp != rt.second);

};

static int md(int year, int month) {
    bool leap = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
    static int tab[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return tab[month] + (leap && month == 2);
}

void update_jiffy(void) {
    jiffy ++;
    if (jiffy % HZ == 0) {
        rt.second ++;
        if (rt.second >= 60) { rt.second = 0; rt.minute ++; }
        if (rt.minute >= 60) { rt.minute = 0; rt.hour ++; }
        if (rt.hour >= 24)   { rt.hour = 0;   rt.day ++;}
        if (rt.day >= md(rt.year, rt.month)) { rt.day = 1; rt.month ++; } 
        if (rt.month >= 13)  { rt.month = 1;  rt.year ++; }
    }
}

void init_i8253(void) {
    int count = FREQ_8253 / HZ;
    // 寄存器（16位Reload register）最高不能超过65536
    assert(count < 65536);

    // 将PIC的0x43 Mode/Command register写为: 00 11 010 0
    // Select channel : Channel 0
    // Access mode : lobyte/hibyte
    // Operating mode : Mode 2 (rate generator), 通常操作系统使用Mode 3(square wave generator)产生IRQ0时钟中断，但使用Mode 2可以得到更精确的时钟频率
    // BCD/Binary mode: 0 = 16-bit binary
    out_byte(PORT_TIME + 3, 0x34);
    // 下面两个写操作写入PIT的Channel 0，设置counter
    // 因为Access mode为lobyte/hibyte，所以两次先后分别写入低8位和高8位
    out_byte(PORT_TIME    , count % 256);
    out_byte(PORT_TIME    , count / 256);   
}

// 详细参见 http://wiki.osdev.org/Programmable_Interval_Timer
// 另外一个简洁的介绍 http://en.wikibooks.org/wiki/X86_Assembly/Programmable_Interval_Timer


int read_rtc(int reg) {
    out_byte(PORT_RTC, reg);
    int ret = in_byte(PORT_RTC + 1);
    return (ret & 0xf) + (ret >> 4) * 10;
}

void get_time(Time *tm) {
    lock();
    memcpy(tm, &rt, sizeof(Time));
    unlock();
}
