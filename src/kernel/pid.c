/*
 * pid.c
 *
 *  Created on: 2014-5-9
 *      Author: kongfy
 */

#include "kernel/pid.h"

int32_t get_free_pid(void)
{
	static int32_t pid = 0;
	return pid++;
}


