#ifndef __FSYS_FS_TYPES_H__
#define __FSYS_FS_TYPES_H__

#include "common.h"

typedef enum FileType {
    Regular = 0,
    Device  = 1,
} FileType;

typedef struct FileInfo
{
    FileType type;
    int dev_id;
    int _count;
} FI;

typedef struct OpenFileTableEntry
{
    off_t offset;
    FI *file;
} OFTE;

typedef struct fm_struct
{
    OFTE *fd[8];
} fm_struct;

#endif /* __FSYS_FS_TYPES_H__ */
