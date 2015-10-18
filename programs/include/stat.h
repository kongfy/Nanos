#ifndef STAT_H
#define STAT_H

#include "types.h"

typedef enum iNode_type {
    PLAIN = 0,
    DIRECTORY = 1,
    DEVICE = 2,
    PIPE = 3,
} iNode_type;

struct stat
{
    size_t size;
    size_t blks;
    iNode_type type;
    int dev_id;
};

int stat(const char *path, struct stat *buf);

#endif /* STAT_H */
