#ifndef __TIME_H__
#define __TIME_H__

#define PORT_TIME 0x40
#define PORT_RTC  0x70
#define FREQ_8253 1193182

#define HZ        100

struct Time {
    int year, month, day;
    int hour, minute, second;
};
typedef struct Time Time;
extern long jiffy;

void get_time(Time *tm);

#endif
