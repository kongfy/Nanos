/*
 * pm.c
 *
 *  Created on: 2014-7-8
 *      Author: kongfy
 */

#include "server/pm.h"
#include "kernel.h"
#include "common.h"

pid_t PM;

extern void pm_server_thread();

void init_pm()
{
    PM = create_kthread(pm_server_thread)->pid;
}
