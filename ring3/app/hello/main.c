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
        int status = waitpid(pid);
        printk("child exit with code : %d\n", status);
    }

    pid = getpid();
    while (1) {
        x++;
        printk("PID %d : hello, world! No.%d\n", pid, x);
        printk("PID %d : now going to sleep\n", pid);
        sleep(1);
        printk("PID %d : wake up!!!\n", pid);
    }

    return 0;
}
