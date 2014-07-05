/*
 * ramdisk.c
 *
 *  Created on: 2014-7-5
 *      Author: kongfy
 */


#include "kernel.h"
#include "common.h"
#include "drivers/hal.h"
#include "drivers/ramdisk.h"

pid_t RAMDISK;

void ramdiskd(void);

void init_ramdisk(void)
{
    RAMDISK = create_kthread(ramdiskd)->pid;


    hal_register("null", RAMDISK, NULL_ID);
    hal_register("zero", RAMDISK, ZERO_ID);
    hal_register("ram", RAMDISK, RAMDISK_ID);
}

