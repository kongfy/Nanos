/*
 * main.c
 *
 *  Created on: 2014-7-8
 *      Author: kongfy
 */

#include "stdio.h"

volatile int x = 10;

int main(int argc, char *argv[])
{
    while (1) {
        x++;
    }

    return 0;
}
