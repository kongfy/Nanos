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

#define MSG_FM_RD 1
#define MSG_FM_WR 2

#define MSG_FM_OPEN   10
#define MSG_FM_LSEEK  11
#define MSG_FM_CLOSE  12
#define MSG_FM_DUP    13
#define MSG_FM_DUP2   14
#define MSG_FM_PIPE   15
#define MSG_FM_READ   16
#define MSG_FM_WRITE  17

typedef struct FMMessage {
    MsgHead header; // header与Message的头部定义保持一致即可(src, dst, type)
    int file_name;
    int fd1, fd2;
    off_t offset;
    int whence;
    uint32_t buf;
    size_t len;
    uint32_t pipefd;
    pid_t req_pid;
    void *dest_addr;
    unsigned int ret;
} FMMessage;

void init_fm();

extern pid_t FM;

#endif /* __SERVER_FM_H__ */
