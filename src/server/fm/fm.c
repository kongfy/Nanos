/*
 * fm.c
 *
 *  Created on: 2014-7-5
 *      Author: kongfy
 */

#include "server/fm.h"
#include "kernel.h"
#include "common.h"

pid_t FM;

extern void fm_server_thread();

void init_fm()
{
    FM = create_kthread(fm_server_thread)->pid;
}

