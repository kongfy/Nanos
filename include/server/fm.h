/*
 * fm.h
 *
 *  Created on: 2014-7-5
 *      Author: kongfy
 */

#ifndef __SERVER_FM_H__
#define __SERVER_FM_H__

#include "kernel.h"
#include "common.h"

#define MSG_FM_RW 1
#define MSG_FM_WR 2

typedef struct FMMessage {
    MsgHead header; // header与Message的头部定义保持一致即可(src, dst, type)
    int file_name;
    pid_t req_pid;
    off_t offset;
    void *buf;
    size_t len;
    size_t ret;
} FMMessage;

void init_fm();

extern pid_t FM;

#endif /* __SERVER_FM_H__ */
