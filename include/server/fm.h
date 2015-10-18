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

#define MSG_FM_INIT    5
#define MSG_FM_COPY    6
#define MSG_FM_EXIT    7

#define MSG_FM_OPEN   10
#define MSG_FM_LSEEK  11
#define MSG_FM_CLOSE  12
#define MSG_FM_DUP    13
#define MSG_FM_DUP2   14
#define MSG_FM_PIPE   15
#define MSG_FM_READ   16
#define MSG_FM_WRITE  17
#define MSG_FM_CHDIR  18
#define MSG_FM_LSDIR  19
#define MSG_FM_MKDIR  20
#define MSG_FM_RMDIR  21
#define MSG_FM_UNLINK 22
#define MSG_FM_STAT   23

typedef struct FMMessage {
    MsgHead header;  // header与Message的头部定义保持一致即可(src, dst, type)
    const char *filename; // Warning: DO NOT change the memory before the operation completed!
    int fd1, fd2;
    off_t offset;
    int whence;
    uint32_t buf;
    size_t len;
    uint32_t pipefd;
    pid_t req_pid;
    pid_t child_pid;
    pid_t usr_pid;
    void *dest_addr;
    int tty;
    int ret;
} FMMessage;

void init_fm();

extern pid_t FM;

#endif /* __SERVER_FM_H__ */
