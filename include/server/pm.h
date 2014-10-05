/*
 * pm.h
 *
 *  Created on: 2014-7-8
 *      Author: kongfy
 */

#ifndef __SERVER_PM_H__
#define __SERVER_PM_H__

#include "kernel.h"
#include "common.h"

#define MSG_PM_FORK 1
#define MSG_PM_EXEC 2

void init_pm();

typedef struct PMMessage {
    MsgHead header; // header与Message的头部定义保持一致即可(src, dst, type)
    uint32_t ret;   // return value, eg. child's pid in fork
} PMMessage;


extern pid_t PM;

#endif /* __SERVER_PM_H__ */
