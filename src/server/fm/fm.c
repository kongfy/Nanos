/*
 * fm.c
 *
 *  Created on: 2014-7-5
 *      Author: kongfy
 */

#include "server/fm.h"
#include "kernel.h"
#include "common.h"
#include "fsys.h"

pid_t FM;

extern void fm_server_thread();
extern void fsysd();

void init_fm()
{
    FM = create_kthread(fm_server_thread)->pid;
    FSYSD = create_kthread(fsysd)->pid;
}

