/*
 * ramdisk.c
 *
 *  Created on: 2014-7-5
 *      Author: kongfy
 */


#include "kernel.h"
#include "common.h"
#include "hal.h"
#include "drivers/ramdisk.h"

pid_t RAMDISK;

void ramdiskd(void);
void ramdisk_setup(void);

void init_ramdisk(void)
{
    ramdisk_setup();
    RAMDISK = create_kthread(ramdiskd)->pid;

    hal_register("null", RAMDISK, NULL_ID);
    hal_register("zero", RAMDISK, ZERO_ID);
    hal_register("ram", RAMDISK, RAMDISK_ID);
}

