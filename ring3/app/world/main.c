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
    printk("my argv is : ");

    int i;
    for (i = 0; i < argc; ++i) {
        printk("%s", argv[i]);
    }

    printk("\n");

    while (1) {
        //printk("!");
    }

    return 0;
}
