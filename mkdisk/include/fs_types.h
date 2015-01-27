#ifndef __FS_TYPES_H__
#define __FS_TYPES_H__

#include <unistd.h>

typedef unsigned int   uint32_t;
typedef          int   int32_t;
typedef unsigned short uint16_t;
typedef          short int16_t;
typedef unsigned char  uint8_t;
typedef unsigned char  bool;

typedef enum iNode_type {
    PLAIN = 0,
    DIRECTORY = 1,
    DEVICE = 2,
    PIPE = 3,
} iNode_type;

typedef struct iNode_entry {
    size_t size;
    size_t blks;
    int type;
    int dev_id;
    int index[15];
} iNode_entry;

typedef struct dir_entry {
    char filename[32];
    int inode;
} dir_entry;

#endif /* __FS_TYPES_H__ */
