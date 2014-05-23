/*
 * test_sem.c
 *
 *  Created on: 2014-5-23
 *      Author: kongfy
 */

#include "test/test_sem.h"
#include "kernel.h"
#include "stdio.h"

#define NBUF 5
#define NR_PROD 3
#define NR_CONS 4

int buf[NBUF], f = 0, r = 0, g = 1;
int last = 0;
Semaphore empty, full, mutex;

void test_producer(void)
{
    while (1) {
        P(&empty);
        P(&mutex);
        if (g % 10000 == 0) {
            printf(".");    // tell us threads are really working
        }
        buf[f++] = g++;
        f %= NBUF;
        V(&mutex);
        V(&full);
    }
}

void test_consumer(void)
{
    int get;
    while (1) {
        P(&full);
        P(&mutex);
        get = buf[r++];
        assert(last == get - 1);   // the products should be strictly increasing
        last = get;
        r %= NBUF;
        V(&mutex);
        V(&empty);
    }
}

void test_sem(void)
{
    init_sem(&full, 0);
    init_sem(&empty, NBUF);
    init_sem(&mutex, 1);

    int i;
    for (i = 0; i < NR_PROD; i++) {
        wakeup(create_kthread(test_producer));
    }
    for (i = 0; i < NR_CONS; i++) {
        wakeup(create_kthread(test_consumer));
    }
}
