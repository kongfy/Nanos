/*
 * schedule.h
 *
 *  Created on: 2014-5-8
 *      Author: kongfy
 */

#ifndef __KERNEL_SCHEDULE_H__
#define __KERNEL_SCHEDULE_H__

#include "kernel/kthread.h"
#include "common.h"

// 调度函数
void schedule(void);

// 是否需要执行调度
extern bool need_sched;

#endif /* __KERNEL_SCHEDULE_H__ */
