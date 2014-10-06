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

uint32_t copy_from_kernel(Thread* pcb, void* dest, void* src, int len);
uint32_t copy_to_kernel(Thread* pcb, void* dest, void* src, int len);
uint32_t strcpy_to_kernel(Thread* pcb, char* dest, char* src);
uint32_t strcpy_from_kernel(Thread* pcb, char* dest, char* src);

#endif /* __KERNEL_H__ */
