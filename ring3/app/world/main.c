/*
 * main.c
 *
 *  Created on: 2014-7-8
 *      Author: kongfy
 */

#include "stdio.h"
#include "unistd.h"

volatile int x = 10;

int main(int argc, char *argv[])
{
    int i;
    for (i = 0; i < argc; ++i) {
        printk("argv[%d] : %s\n", i, argv[i]);
    }

    pid_t pid = getpid();
    printk("PID %d : now going to sleep\n", pid);
    sleep(5);
    printk("PID %d : wake up!!!\n", pid);

    while (1) {
        pid = fork();

        if (0 == pid) {
            pid_t pid = getpid();
            printk("I'm PID : %d \n", pid);
        } else {
            // divide zero
            int zero = 0;
            return 10 / zero;
        }
    }

    return -1; // just for waitpid reture value test.
}
