#include "fsys.h"
#include "fs_types.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define INODE_MAP 0
#define INODE (INODE_MAP + ((INODE_NUM >> 3) / BLK_SIZE + 1) * BLK_SIZE)
#define BLK_MAP (INODE + (sizeof(iNode) * INODE_NUM / BLK_SIZE + 1) * BLK_SIZE)
#define BLOCK (BLK_MAP + ((BLK_NUM >> 3) / BLK_SIZE + 1) * BLK_SIZE)
#define END (BLOCK + BLK_SIZE * BLK_NUM)

static uint8_t buf[BLK_SIZE];

int fd_out;

size_t dev_read(off_t offset, void *buf, size_t count)
{
    return pread(fd_out, buf, count, offset);
}

size_t dev_write(off_t offset, void *buf, size_t count)
{
    return pwrite(fd_out, buf, count, offset);
}

static inline
int min(int a, int b)
{
    if (a < b) return a;
    return b;
}

int init_disk()
{
    // magic number is my birthday :D
    char magic[4] = {0x19, 0x90, 0x08, 0x02};
    if (pwrite(fd_out, magic, 4, END) != 4) {
        return -1;
    }
    return 0;
}

int get_free_inode()
{
    int blks = (INODE_NUM >> 3) / BLK_SIZE;
    if (((INODE_NUM >> 3) % BLK_SIZE) > 0) blks++;

    int b = 0;
    for (b = 0; b < blks; ++b) {
        dev_read(INODE_MAP + b * BLK_SIZE, buf, BLK_SIZE);

        int i = 0;
        for (i = (b * BLK_SIZE) << 3; i < min(((b+1) * BLK_SIZE) << 3, INODE_NUM); ++i) {
            int t = i - ((b * BLK_SIZE) << 3);
            uint8_t mask = 1U << (t % 8);

            if (!(buf[t >> 3] & mask)) {
                buf[t >> 3] |= mask;
                dev_write(INODE_MAP + b * BLK_SIZE + (t >> 3), &buf[t >> 3], 1);
                return i;
            }
        }
    }

    printf("FATAL : No available inode.\n");
    exit(-1);
    return -1;
}

int get_free_block()
{
    int blks = (BLK_NUM >> 3) / BLK_SIZE;
    if (((BLK_NUM >> 3) % BLK_SIZE) > 0) blks++;

    int b = 0;
    for (b = 0; b < blks; ++b) {
        dev_read(BLK_MAP + b * BLK_SIZE, buf, BLK_SIZE);

        int i = 0;
        for (i = (b * BLK_SIZE) << 3; i < min(((b+1) * BLK_SIZE) << 3, BLK_NUM); ++i) {
            int t = i - ((b * BLK_SIZE) << 3);
            uint8_t mask = 1U << (t % 8);

            if (!(buf[t >> 3] & mask)) {
                buf[t >> 3] |= mask;
                dev_write(BLK_MAP + b * BLK_SIZE + (t >> 3), &buf[t >> 3], 1);
                return i;
            }
        }
    }

    printf("FATAL : No available block.\n");
    exit(-1);
    return -1;
}

void clear_inode_map(int index)
{
    uint8_t c;
    dev_read(INODE_MAP + (index >> 3), &c, 1);
    c &= ~(1U << index % 8);
    dev_write(INODE_MAP + (index >> 3), &c, 1);
}

void clear_block_map(int index)
{
    uint8_t c;
    dev_read(BLK_MAP + (index >> 3), &c, 1);
    c &= ~(1U << index % 8);
    dev_write(BLK_MAP + (index >> 3), &c, 1);
}

iNode load_inode(int index)
{
    iNode temp;
    dev_read(INODE + index * sizeof(iNode), &temp, sizeof(iNode));
    return temp;
}

void save_inode(int index, iNode *inode)
{
    dev_write(INODE + index * sizeof(iNode), inode, sizeof(iNode));
}

size_t read_block(int index, void *buffer)
{
    return dev_read(BLOCK + index * BLK_SIZE, buffer, BLK_SIZE);
}

size_t write_block(int index, void *buffer)
{
    return dev_write(BLOCK + index * BLK_SIZE, buffer, BLK_SIZE);
}
