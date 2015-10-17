#ifndef __FSYS_FSYSD_H__
#define __FSYS_FSYSD_H__

#include "kernel.h"

#define MSG_FSYS_READ_BY_FILENAME 5
#define MSG_FSYS_INODE_FOR_FILENAME 6
#define MSG_FSYS_CHDIR 7
#define MSG_FSYS_LSDIR 8
#define MSG_FSYS_MKDIR 9
#define MSG_FSYS_RMDIR 10

typedef struct FSYSMessage {
    MsgHead header;  // header与Message的头部定义保持一致即可(src, dst, type)
    pid_t req_pid;
    const char *filename; // Warning: DO NOT change the memory before the operation completed!
    iNode *inode;
    int pwd;
    uint8_t *buf;
    off_t offset;
    size_t len;
    int ret;
} FSYSMessage;

extern pid_t FSYSD;

#endif /* __FSYS_FSYSD_H__ */
