/*
 * ramdisk.h
 *
 *  Created on: 2014-7-5
 *      Author: kongfy
 */

#ifndef __DRIVERS_RAMDISK_H__
#define __DRIVERS_RAMDISK_H__

#define NR_MAX_FILE 8
#define NR_FILE_SIZE (128 * 1024)
#define NR_DISK_SIZE (NR_MAX_FILE * NR_FILE_SIZE)

#define RAMDISK_ID 2
#define NULL_ID 0
#define ZERO_ID 1

extern pid_t RAMDISK;


#endif /* __DRIVERS_RAMDISK_H__ */
