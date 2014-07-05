/*
 * test_ramdisk.c
 *
 *  Created on: 2014-7-5
 *      Author: kongfy
 */

#include "test/test_ramdisk.h"

#include "kernel.h"
#include "common.h"
#include "hal.h"
#include "server.h"

void reading()
{
    int i;
    char buf[20];

    for (i = 0; i < 3; ++i) {
        Message m;
        FMMessage *msg = (FMMessage *)&m;

        m.type = MSG_FM_RW;
        msg->file_name = i;
        msg->buf = buf;
        msg->offset = 0;
        msg->len = 20;

        send(FM, &m);
        receive(FM, &m);

        printf("%d :%s\n", msg->ret, buf);
    }

    printf("Reading test end!\n");
}

void test_ramdisk()
{
    create_kthread(reading);
}
