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
#define MSG_PM_EXIT 3
#define MSG_PM_WAITPID 4
#define MSG_PM_KILL 5

void init_pm();

typedef struct PMMessage {
    MsgHead header; // header与Message的头部定义保持一致即可(src, dst, type)
    const char *filename;
    uint32_t argv;
    int32_t status; // used in exit syscall & kill
    pid_t pid;      // used in waitpid syscall & kill
    int32_t ret;   // return value, eg. child's pid in fork & status in waitpid
} PMMessage;


extern pid_t PM;

void kill_thread(Thread * thread, int irq); // called when user thread trigger exceptions

#endif /* __SERVER_PM_H__ */
