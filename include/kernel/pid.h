/*
 * pid.h
 *
 *  Created on: 2014-5-9
 *      Author: kongfy
 */

#ifndef __KERNEL_PID_H__
#define __KERNEL_PID_H__

#include "common.h"

#define MAX_PROCESS 32

// 最小的可用pid，如果没有可用pid，返回-1
int32_t get_free_pid(void);

#endif /* __KERNEL_PID_H__ */
