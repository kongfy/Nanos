/*
 * mm.h
 *
 *  Created on: 2014-7-8
 *      Author: kongfy
 */

#ifndef __SERVER_MM_H__
#define __SERVER_MM_H__

#include "kernel.h"
#include "common.h"

#define MSG_MM_CREATE_VM 1
#define MSG_MM_MMAP 2
#define MSG_MM_FORK 3
#define MSG_MM_REVOKE_VM 4

void init_mm();

typedef struct MMMessage {
    MsgHead header; // header与Message的头部定义保持一致即可(src, dst, type)
    pid_t pid;
    pid_t child_pid; // used in system call : fork
    uint32_t vaddr;
    uint32_t len;
    uint32_t read_write;
} MMMessage;


extern pid_t MM;

#endif /* __SERVER_MM_H__ */
