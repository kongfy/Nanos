/*
 * server.c
 *
 *  Created on: 2014-7-8
 *      Author: kongfy
 */

#include "kernel.h"
#include "server.h"

#include "hal.h"
#include "elf.h"

void create_first_process();

// PM服务器线程
void pm_server_thread()
{
    create_first_process();

    while (1) {
    }

    assert(0);
}
