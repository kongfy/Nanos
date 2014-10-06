/*
 * main.c
 *
 *  Created on: 2014-7-8
 *      Author: kongfy
 */

#include "stdio.h"
#include "unistd.h"

volatile int x = 10;

char *arga = "hello";
char *argb = "world";

int main(int argc, char *argv[])
{
    pid_t pid = fork();

    if (0 == pid) {
        printk("I'm child\n");

        char *argv[] = {arga, argb, (char *)0};
        exec(1, argv);
    } else {
        printk("child process pid : %d\n", pid);
    }

    pid = getpid();
    while (1) {
        x++;
        if (x % 1000000 == 0) {
            printk("hello from process : %d\n", pid);
        }
    }

    return 0;
}
