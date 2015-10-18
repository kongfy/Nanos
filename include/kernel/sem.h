#ifndef __KERNEL_SEM_H__
#define __KERNEL_SEM_H__

#include "list.h"

typedef struct Semaphore
{
    volatile int count;
    list_head queue;
} Semaphore;

// 信号量初始化
void init_sem(Semaphore *sem, int value);

// P、V操作
void P(Semaphore *sem);
void V(Semaphore *sem);

#endif /* __KERNEL_SEM_H__ */
