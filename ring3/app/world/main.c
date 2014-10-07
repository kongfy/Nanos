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

    return -1; // just for waitpid reture value test.
}
