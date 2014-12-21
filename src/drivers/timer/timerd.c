/*
 * timerd.c
 *
 *  Created on: 2014-6-24
 *      Author: kongfy
 */

#include "kernel.h"
#include "common.h"

#include "drivers/time.h"

void init_alarm(void);
void update_alarm(void);
void setup_alarm(pid_t pid, uint32_t seconds);

// 时钟守护线程
void timerd(void)
{
    init_alarm();

    Message m;
    TIMEMessage *msg = (TIMEMessage *)&m;

    while (1) {
        receive(ANY, &m);

        switch(m.type) {
        case MSG_TIME_ALARM:
            setup_alarm(msg->header.src, msg->seconds);
            break;
        case MSG_TIME_UP:
            /* this is a bottom half
               just forword the time up message */
            assert(MSG_HWINTR == msg->header.src);
            send(msg->pid, &m);
            break;
        default:
            assert(0);
        }
    }
}

typedef struct {
    list_head queue;
    pid_t pid;
    uint32_t seconds;
} Alarm;

#define NR_ALARM 256
static list_head alarmq, freeq;
static Alarm alarms[NR_ALARM];

void init_alarm(void)
{
    INIT_LIST_HEAD(&alarmq);
    INIT_LIST_HEAD(&freeq);

    Alarm *p = alarms;
    while (p < &alarms[NR_ALARM]) {
        list_add_tail(&p->queue, &freeq);
        p++;
    }
}

void setup_alarm(pid_t pid, uint32_t seconds)
{
    if (seconds <= 0) {
        Message m;
        TIMEMessage *msg = (TIMEMessage *)&m;
        msg->header.type = MSG_TIME_UP;
        msg->seconds = 0;
        msg->pid = pid;

        send(TIME, &m);
        return;
    }

    assert(!list_empty(&freeq));

    lock();

    Alarm* alarm = list_entry(freeq.next, Alarm, queue);
    alarm->pid = pid;
    list_del(&alarm->queue);

    list_head *t = &alarmq;
    Alarm *pos;
    list_for_each_entry(pos, &alarmq, queue) {
        if (seconds < pos->seconds) {
            t = &pos->queue;
            pos->seconds -= seconds;
            break;
        } else {
            seconds -= pos->seconds;
        }
    }

    alarm->seconds = seconds;
    list_add_before(t, &alarm->queue);

    unlock();
}

void update_alarm()
{
    if (list_empty(&alarmq)) {
        return;
    }

    lock();

    Alarm *head = list_entry(alarmq.next, Alarm, queue);
    head->seconds -= 1;

    Alarm *pos, *n;
    list_for_each_entry_safe(pos, n, &alarmq, queue) {
        if (pos->seconds <= 0) {
            Message m;
            TIMEMessage *msg = (TIMEMessage *)&m;
            msg->header.type = MSG_TIME_UP;
            msg->seconds = 0;
            msg->pid = pos->pid;

            send(TIME, &m);
            list_del(&pos->queue);
            list_add_tail(&pos->queue, &freeq);
        } else {
            break;
        }
    }

    unlock();
}
