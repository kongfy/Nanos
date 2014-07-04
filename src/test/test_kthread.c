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
    int x = 0;
    while (1) {
        if (x % 100000 == 0) {
            printf("a");
        }
        x ++;
    }
}

void ab_print_B()
{
    int x = 0;
    while (1) {
        if (x % 100000 == 0) {
            printf("b");
        }
        x ++;
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

static Thread *sleep_wakeup_a, *sleep_wakeup_b, *sleep_wakeup_c, *sleep_wakeup_d;

void sleep_wakeup_A () {
    int x = 0;

    while(1) {
        if(x % 100000 == 0) {
            printf("a");
            wakeup(sleep_wakeup_b);
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
            wakeup(sleep_wakeup_c);
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
            wakeup(sleep_wakeup_d);
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
            wakeup(sleep_wakeup_a);
            sleep();
        }
        x++;
    }
}

void test_sleep_wakeup()
{
    sleep_wakeup_d = create_kthread(sleep_wakeup_D);
    sleep_wakeup_c = create_kthread(sleep_wakeup_C);
    sleep_wakeup_b = create_kthread(sleep_wakeup_B);
    sleep_wakeup_a = create_kthread(sleep_wakeup_A);
}

/* <============================= 测试用例分界线 ==============================> */

static Thread *hungry_a, *hungry_b;

void hungry_A()
{
    while (TRUE) {
        printf("a");
        wakeup(hungry_b);
        sleep();
    }
}

void hungry_B()
{
    sleep();

    while (TRUE) {
        printf("b");
        wakeup(hungry_a);
        sleep();
    }
}

void hungry_C()
{
    int x = 0;

    while (TRUE) {
        if (x % 100000 == 0) {
            printf("c");
        }
        x++;
    }
}

void test_hungry()
{
    hungry_b = create_kthread(hungry_B);
    hungry_a = create_kthread(hungry_A);
    create_kthread(hungry_C);
}
