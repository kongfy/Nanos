#ifndef __FSYS_FS_TYPES_H__
#define __FSYS_FS_TYPES_H__

#include "common.h"
#include "hal.h"

#define NR_FD 8

#define FSYSDEV "hda"
#define FSYSOFF (512 << 10)
#define INODE_NUM 4096
#define BLK_NUM 65536
#define BLK_SIZE 1024
#define MAX_PATH_LEN 2048

typedef enum iNode_type {
    PLAIN = 0,
    DIRECTORY = 1,
    DEVICE = 2,
    PIPE = 3,
} iNode_type;

typedef struct iNode_entry {
    size_t size;
    size_t blks;
    iNode_type type;
    int dev_id;
    int index[15];
} iNode_entry;

typedef struct iNode_in_mem {
    int index;
    iNode_entry entry;
} iNode;

typedef struct dir_entry {
    char filename[32];
    int inode;
} dir_entry;

typedef enum FileType {
    Reg = 0,
    Dev = 1,
} FileType;

typedef struct FileInfo
{
    FileType type;
    Device *dev;
    iNode inode;
    int _count;
} FI;

typedef struct OpenFileTableEntry
{
    off_t offset;
    FI *file;
    int _count;
} OFTE;

typedef struct fm_struct
{
    OFTE *fd[NR_FD];
    int pwd;
} fm_struct;

#endif /* __FSYS_FS_TYPES_H__ */
