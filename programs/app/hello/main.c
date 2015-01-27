/*
 * main.c
 *
 *  Created on: 2014-7-8
 *      Author: kongfy
 */

#include "stdio.h"

int main(int argc, char *argv[])
{
    printf("hello, world!\n");

    int i = 0;
    for (i  = 0; i < argc; ++i) {
        printf("argv[%d] : %s\n", i, argv[i]);
    }

    return 0;
}
