/*
 * test_kthread.h
 *
 *  Created on: 2014-5-16
 *      Author: kongfy
 */

#ifndef __TEST_TEST_KTHREAD_H__
#define __TEST_TEST_KTHREAD_H__

// 两个线程交替执行，打印出aaaaaaaabbbbbbbbb
void test_ab_print();

// PID分配测试，不断创建、销毁线程，打印获得的PID
void test_pid_alloc();

// 内核线程sleep() wakeup()测试，交替打印abcdabcd
void test_sleep_wakeup();

// 调度饥饿测试
void test_hungry();

#endif /* __TEST_TEST_KTHREAD_H__ */
