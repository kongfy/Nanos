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

void pm_server_thread();
void init_waitpid_structure();

void init_pm()
{
    init_waitpid_structure();

    PM = create_kthread(pm_server_thread)->pid;
}
