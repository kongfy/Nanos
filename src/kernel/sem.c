#include "kernel/sem.h"
#include "kernel.h"

void init_sem(Semaphore *sem, int value)
{
    sem->count = value;
    INIT_LIST_HEAD(&sem->queue);
}

void P(Semaphore *sem)
{
    lock();

    sem->count --;
    if (sem->count < 0) {
        list_add_tail(&current->semq, &sem->queue); // You should add ListHead semq in the Thread structure
        sleep(); // sleep!
        NOINTR;
    }

    unlock();
}

void V(Semaphore *sem)
{
    lock();

    sem->count ++;
    if (sem->count <= 0) {
        assert(!list_empty(&sem->queue));
        Thread *t = list_entry(sem->queue.next, Thread, semq);
        list_del(sem->queue.next);
        wakeup(t);
    }

    unlock();
}
