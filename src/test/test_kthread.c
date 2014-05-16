/*
 * test_kthread.c
 *
 *  Created on: 2014-5-16
 *      Author: kongfy
 */


#include "test/test_kthread.h"

#include "common.h"
#include "kernel.h"
#include "stdio.h"

/* <============================= 测试用例分界线 ==============================> */

void ab_print_A()
{
    while (TRUE) {
        printf("a");
    }
}

void ab_print_B()
{
    while (TRUE) {
        printf("b");
    }
}

void test_ab_print()
{
    create_kthread(ab_print_A);
    create_kthread(ab_print_B);
}

/* <============================= 测试用例分界线 ==============================> */

void pid_alloc()
{
    Thread *tcb = create_kthread(pid_alloc);
    if (!tcb) {
        printf("create kthread failed!\n");
    } else {
        printf("%d\n", tcb->pid);
    }
}

void test_pid_alloc()
{
    Thread *tcb = create_kthread(pid_alloc);
    if (!tcb) {
        printf("create kthread failed!\n");
    } else {
        printf("%d\n", tcb->pid);
    }
}

/* <============================= 测试用例分界线 ==============================> */

static Thread *tcb_a, *tcb_b, *tcb_c, *tcb_d;

void sleep_wakeup_A () {
    int x = 0;

    while(1) {
        if(x % 100000 == 0) {
            printf("a");
            wakeup(tcb_b);
            sleep();
        }
        x++;
    }
}

void sleep_wakeup_B () {
    int x = 0;

    sleep();
    while(1) {
        if(x % 100000 == 0) {
            printf("b");
            wakeup(tcb_c);
            sleep();
        }
        x++;
    }
}

void sleep_wakeup_C () {
    int x = 0;

    sleep();
    while(1) {
        if(x % 100000 == 0) {
            printf("c");
            wakeup(tcb_d);
            sleep();
        }
        x++;
    }
}

void sleep_wakeup_D () {
    int x = 0;

    sleep();
    while(1) {
        if(x % 100000 == 0) {
            printf("d");
            wakeup(tcb_a);
            sleep();
        }
        x++;
    }
}

void test_sleep_wakeup()
{
    tcb_d = create_kthread(sleep_wakeup_D);
    tcb_c = create_kthread(sleep_wakeup_C);
    tcb_b = create_kthread(sleep_wakeup_B);
    tcb_a = create_kthread(sleep_wakeup_A);
}

/* <============================= 测试用例分界线 ==============================> */

