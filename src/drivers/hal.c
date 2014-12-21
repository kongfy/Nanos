#include "hal.h"
#include "kernel.h"
#include "string.h"
#include "common.h"

#define NR_DEV 64

static Device dev_pool[NR_DEV];
static list_head free, devices;


void init_hal(void) {
    int i = 0;
    INIT_LIST_HEAD(&free);
    for (i = 0; i < NR_DEV; i ++) {
        list_add_before(&free, &dev_pool[i].list);
    }
    INIT_LIST_HEAD(&devices);
}

void hal_register(const char *name, pid_t pid, int dev_id) {
    lock();
    if (list_empty(&free)) {
        panic("no room for more device");
    }
    Device *dev = list_entry(free.next, Device, list);
    list_del(&dev->list);
    dev->name = name;
    dev->pid = pid;
    dev->dev_id = dev_id;
    list_add_before(&devices, &dev->list);
    unlock();
}

void hal_list(void) {
    lock();
    list_head *it;
    printf("listing all registed devices:\n");
    list_for_each(it, &devices) {
        Device *dev = list_entry(it, Device, list);
        printf("%s #%d, #%d\n", dev->name, dev->pid, dev->dev_id);
    }
    unlock();
}

Device *hal_get(const char *name) {
    lock();
    list_head*it;
    list_for_each(it, &devices) {
        Device *dev = list_entry(it, Device, list);
        if (strcmp(dev->name, name) == 0) {
            unlock();
            return dev;
        }
    }
    unlock();
    return NULL;
}

static size_t
dev_rw(int type, Device *dev, pid_t reqst_pid, off_t offset, void *buf, size_t len) {
    Message m;
    DevMessage *Msg = (DevMessage *)&m;
    assert(sizeof(DevMessage) <= sizeof(Message)); // Message结构体不能定义得太小

    m.type = type;
    Msg->dev_id = dev->dev_id;
    Msg->offset = offset;
    Msg->buf = buf;
    Msg->len = len;
    Msg->req_pid = reqst_pid;
    send(dev->pid, (Message*)&m);
    receive(dev->pid, (Message*)&m);

    return Msg->ret;
}

size_t
dev_read(Device *dev, pid_t reqst_pid, off_t offset, void *buf, size_t len) {
    return dev_rw(MSG_DEVRD, dev, reqst_pid, offset, buf, len);
}

size_t
dev_write(Device *dev, pid_t reqst_pid, off_t offset, void *buf, size_t len) {
    return dev_rw(MSG_DEVWR, dev, reqst_pid, offset, buf, len);
}

