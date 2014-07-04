#ifndef __DRIVERS_HAL_H__
#define __DRIVERS_HAL_H__

#define MSG_DEVRD  1
#define MSG_DEVWR  2

#include "kernel/list.h"
#include "common.h"

typedef struct MsgHead {
    int type; // 消息的类型
    pid_t src, dest; // 消息发送者和接收者的pid
} MsgHead;

struct DevMessage {
    MsgHead header; // header与Message的头部定义保持一致即可(src, dst, type)
    int dev_id;
    pid_t req_pid;
    off_t offset;
    void *buf;
    size_t len;
    int ret;
};
typedef struct DevMessage DevMessage;

struct Device {
    const char *name;
    pid_t pid;
    int dev_id;
    
    list_head list;
};
typedef struct Device Device;

size_t dev_read(Device *dev, pid_t reqst_pid, off_t offset, void *buf, size_t count);
size_t dev_write(Device *dev, pid_t reqst_pid, off_t offset, void *buf, size_t count);

void hal_register(const char *name, pid_t pid, int dev_id);
Device *hal_get(const char *name);
void hal_list(void);

#endif
