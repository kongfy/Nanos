#ifndef __TIME_H__
#define __TIME_H__

#define PORT_TIME 0x40
#define PORT_RTC  0x70
#define FREQ_8253 1193182

#define HZ        100

#include "kernel.h"
#include "common.h"

struct Time {
    int year, month, day;
    int hour, minute, second;
};
typedef struct Time Time;
extern long jiffy;

extern pid_t TIME;

void get_time(Time *tm);

#define MSG_TIME_ALARM 1
#define MSG_TIME_UP 2

typedef struct TIMEMessage {
    MsgHead header; // header与Message的头部定义保持一致即可(src, dst, type)
    uint32_t seconds;
    pid_t pid;
} TIMEMessage;

#endif
