/*
 * pid.h
 *
 *  Created on: 2014-5-9
 *      Author: kongfy
 */

#ifndef __KERNEL_PID_H__
#define __KERNEL_PID_H__

#include "common.h"

// 简化PID实现，使PID可以直接作为PCB数组下标实现
// 使通过PID查找PCB变得容易，但是限制了MAX_PID和MAX_PROCESS的关系
#define MAX_PROCESS 256
#define MAX_PID (MAX_PROCESS - 1)

// 初始化pid
void init_pid(void);

// 检查pid是否被占用
bool is_pid_available(pid_t pid);

// 最小的可用pid，如果没有可用pid，返回-1
pid_t get_free_pid(void);

// 释放pid
void free_pid(pid_t pid);

#endif /* __KERNEL_PID_H__ */
