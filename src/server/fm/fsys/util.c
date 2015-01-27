#include "fsys.h"
#include "hal.h"

#define INODE_MAP 0
#define INODE (INODE_MAP + ((INODE_NUM >> 3) / BLK_SIZE + 1) * BLK_SIZE)
#define BLK_MAP (INODE + (sizeof(iNode_entry) * INODE_NUM / BLK_SIZE + 1) * BLK_SIZE)
#define BLOCK (BLK_MAP + ((BLK_NUM >> 3) / BLK_SIZE + 1) * BLK_SIZE)
#define END (BLOCK + BLK_SIZE * BLK_NUM)

Device *fsys_dev;
static uint8_t buf[BLK_SIZE];

static
size_t raw_read(off_t offset, void *buf, size_t count, pid_t req_pid)
{
    return dev_read_block(fsys_dev, req_pid, offset, buf, count);
}

static
size_t raw_write(off_t offset, void *buf, size_t count, pid_t req_pid)
{
    return dev_write_block(fsys_dev, req_pid, offset, buf, count);
}

static inline
int min(int a, int b)
{
    if (a < b) return a;
    return b;
}

int get_free_inode()
{
    int blks = (INODE_NUM >> 3) / BLK_SIZE;
    if (((INODE_NUM >> 3) % BLK_SIZE) > 0) blks++;

    int b = 0;
    for (b = 0; b < blks; ++b) {
        raw_read(INODE_MAP + b * BLK_SIZE, buf, BLK_SIZE, FSYSD);

        int i = 0;
        for (i = (b * BLK_SIZE) << 3; i < min(((b+1) * BLK_SIZE) << 3, INODE_NUM); ++i) {
            int t = i - ((b * BLK_SIZE) << 3);
            uint8_t mask = 1U << (t % 8);

            if (!(buf[t >> 3] & mask)) {
                buf[t >> 3] |= mask;
                raw_write(INODE_MAP + b * BLK_SIZE + (t >> 3), &buf[t >> 3], 1, FSYSD);
                return i;
            }
        }
    }

    panic("FATAL : No available inode.\n");
    return -1;
}

int get_free_block()
{
    int blks = (BLK_NUM >> 3) / BLK_SIZE;
    if (((BLK_NUM >> 3) % BLK_SIZE) > 0) blks++;

    int b = 0;
    for (b = 0; b < blks; ++b) {
        raw_read(BLK_MAP + b * BLK_SIZE, buf, BLK_SIZE, FSYSD);

        int i = 0;
        for (i = (b * BLK_SIZE) << 3; i < min(((b+1) * BLK_SIZE) << 3, BLK_NUM); ++i) {
            int t = i - ((b * BLK_SIZE) << 3);
            uint8_t mask = 1U << (t % 8);

            if (!(buf[t >> 3] & mask)) {
                buf[t >> 3] |= mask;
                raw_write(BLK_MAP + b * BLK_SIZE + (t >> 3), &buf[t >> 3], 1, FSYSD);
                return i;
            }
        }
    }

    panic("FATAL : No available block.\n");
    return -1;
}

void clear_inode_map(int index)
{
    uint8_t c;
    raw_read(INODE_MAP + (index >> 3), &c, 1, FSYSD);
    c &= ~(1U << index % 8);
    raw_write(INODE_MAP + (index >> 3), &c, 1, FSYSD);
}

void clear_block_map(int index)
{
    uint8_t c;
    raw_read(BLK_MAP + (index >> 3), &c, 1, FSYSD);
    c &= ~(1U << index % 8);
    raw_write(BLK_MAP + (index >> 3), &c, 1, FSYSD);
}

iNode_entry load_inode(int index)
{
    iNode_entry temp;
    raw_read(INODE + index * sizeof(iNode_entry), &temp, sizeof(iNode_entry), FSYSD);
    return temp;
}

void save_inode(int index, iNode_entry *inode)
{
    raw_write(INODE + index * sizeof(iNode_entry), inode, sizeof(iNode_entry), FSYSD);
}

size_t read_block(int index, void *buffer)
{
    return raw_read(BLOCK + index * BLK_SIZE, buffer, BLK_SIZE, FSYSD);
}

size_t write_block(int index, void *buffer)
{
    return raw_write(BLOCK + index * BLK_SIZE, buffer, BLK_SIZE, FSYSD);
}
