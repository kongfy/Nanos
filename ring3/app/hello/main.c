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
    pid_t pid = getpid();

    while (1) {
        x++;
        if (x % 1000000 == 0) {
            printk("hello from process : %d\n", pid);
        }
    }

    return 0;
}
