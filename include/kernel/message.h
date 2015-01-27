/*
 * message.h
 *
 *  Created on: 2014-5-23
 *      Author: kongfy
 */

#ifndef __KERNEL_MESSAGE_H__
#define __KERNEL_MESSAGE_H__

#include "common.h"

#define ANY -1
#define MSG_SZ 128
#define MSG_HWINTR -1

typedef struct Message {
    int type; // 消息的类型
    pid_t src, dest; // 消息发送者和接收者的pid
    char payload[MSG_SZ];
} Message;

typedef struct MsgHead {
    int type; // 消息的类型
    pid_t src, dest; // 消息发送者和接收者的pid
} MsgHead;

void send(pid_t dst, Message *m);
void receive(pid_t src, Message *m);

#endif /* __KERNEL_MESSAGE_H__ */
