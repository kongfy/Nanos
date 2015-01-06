#ifndef __FSYS_FS_TYPES_H__
#define __FSYS_FS_TYPES_H__

#include "common.h"
#include "hal.h"

#define NR_FD 8

typedef enum FileType {
    Reg = 0,
    Dev = 1,
} FileType;

typedef struct FileInfo
{
    FileType type;
    Device *dev;
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
} fm_struct;

#endif /* __FSYS_FS_TYPES_H__ */
