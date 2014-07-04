/*
 * kernel.h
 *
 *  Created on: 2014-5-8
 *      Author: kongfy
 */

#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "kernel/kthread.h"
#include "kernel/schedule.h"
#include "kernel/sem.h"
#include "kernel/message.h"

extern uint8_t logo_i386[]; // i386 Manul Logo...
void add_irq_handle(int irq, void (*func)(void)); // 添加中断处理函数

void copy_from_kernel(Thread* pcb, void* dest, void* src, int len);
void copy_to_kernel(Thread* pcb, void* dest, void* src, int len);
void strcpy_to_kernel(Thread* pcb, char* dest, char* src);
void strcpy_from_kernel(Thread* pcb, char* dest, char* src);

#endif /* __KERNEL_H__ */
