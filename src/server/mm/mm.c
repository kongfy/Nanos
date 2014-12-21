/*
 * mm.c
 *
 *  Created on: 2014-7-8
 *      Author: kongfy
 */

#include "server/mm.h"
#include "kernel.h"
#include "common.h"

pid_t MM;

extern void mm_server_thread();

void init_mm()
{
    MM = create_kthread(mm_server_thread)->pid;
}
