#ifndef __HAL_H__
#define __HAL_H__

#define MSG_DEVRD  200
#define MSG_DEVWR  201

#include "common.h"
#include "kernel/list.h"
#include "kernel/message.h"

struct DevMessage {
    MsgHead header; // header与Message的头部定义保持一致即可(src, dst, type)
    int dev_id;
    pid_t req_pid;
    off_t offset;
    void *buf;
    size_t len;
    size_t ret;
};
typedef struct DevMessage DevMessage;

struct Device {
    const char *name;
    pid_t pid;
    int dev_id;
    
    list_head list;
};
typedef struct Device Device;

void dev_read(Device *dev, pid_t reqst_pid, off_t offset, void *buf, size_t len);
void dev_write(Device *dev, pid_t reqst_pid, off_t offset, void *buf, size_t len);
size_t dev_read_block(Device *dev, pid_t reqst_pid, off_t offset, void *buf, size_t len);
size_t dev_write_block(Device *dev, pid_t reqst_pid, off_t offset, void *buf, size_t len);

void hal_register(const char *name, pid_t pid, int dev_id);
Device *hal_get(const char *name);
void hal_list(void);

#endif
