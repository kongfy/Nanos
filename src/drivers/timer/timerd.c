/*
 * timerd.c
 *
 *  Created on: 2014-6-24
 *      Author: kongfy
 */

#include "kernel.h"
#include "common.h"

// 时钟守护线程
void timerd(void)
{
    Message m;

    while (1) {
        receive(ANY, &m);

        switch(m.type) {
        default:
            assert(0);
        }
    }
}
