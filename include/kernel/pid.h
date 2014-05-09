/*
 * pid.h
 *
 *  Created on: 2014-5-9
 *      Author: kongfy
 */

#ifndef __KERNEL_PID_H__
#define __KERNEL_PID_H__

#include "common.h"

#define MAX_PROCESS 256
#define MAX_PID (MAX_PROCESS - 1)

// 初始化pid
void init_pid(void);

// 最小的可用pid，如果没有可用pid，返回-1
pid_t get_free_pid(void);

// 释放pid
void free_pid(pid_t pid);

#endif /* __KERNEL_PID_H__ */
