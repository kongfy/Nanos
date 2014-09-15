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
#define MSG_MM_ALLOC_PAGES 2

void init_mm();

typedef struct MMMessage {
    MsgHead header; // header与Message的头部定义保持一致即可(src, dst, type)
    pid_t pid;
    unsigned int vaddr;
    unsigned int memsz;
    unsigned int paddr; // return value
} MMMessage;


extern pid_t MM;

#endif /* __SERVER_MM_H__ */
