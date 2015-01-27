#include "fsys.h"
#include "string.h"

#define BLOCKS_PER_BLOCK (BLK_SIZE / sizeof(int))
#define DIRECT_BLK 12
#define L1_BLK (DIRECT_BLK + BLOCKS_PER_BLOCK)
#define L2_BLK (L1_BLK + BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK)
#define L3_BLK (L2_BLK + BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK)

int get_free_inode();
int get_free_block();
void clear_inode_map(int index);
void clear_block_map(int index);
iNode_entry load_inode(int index);
void save_inode(int index, iNode_entry *inode);
size_t read_block(int index, void *buffer);
size_t write_block(int index, void *buffer);

static uint8_t block_data[BLK_SIZE], block_index[BLK_SIZE];
static char path[MAX_PATH_LEN];

static inline
int min(int a, int b)
{
    if (a < b) return a;
    return b;
}

static
int get_blk_for_inode(uint32_t blk, iNode_entry *inode)
{
    if (blk < DIRECT_BLK) {
        return inode->index[blk];
    } else if (blk < L1_BLK) {
        int index = blk - DIRECT_BLK;
        read_block(inode->index[12], block_index);
        return *((int *)block_index + index);
    } else if (blk < L2_BLK) {
        int index = (blk - L1_BLK) / BLOCKS_PER_BLOCK;
        read_block(inode->index[13], block_index);
        read_block(*((int *)block_index + index), block_index);
        index = (blk - L1_BLK) % BLOCKS_PER_BLOCK;
        return *((int *)block_index + index);
    } else if (blk < L3_BLK) {
        int index = (blk - L2_BLK) / (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK);
        read_block(inode->index[14], block_index);
        read_block(*((int *)block_index + index), block_index);
        index = (blk - L2_BLK) % (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK) / BLOCKS_PER_BLOCK;
        read_block(*((int *)block_index + index), block_index);
        index = (blk - L2_BLK) % (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK) % BLOCKS_PER_BLOCK;
        return *((int *)block_index + index);
    } else {
        return -1;
    }
}

static
iNode search_dir(char *filename, iNode *inode)
{
    iNode temp;
    if (inode->entry.type != DIRECTORY) {
        temp.index = -1;
        return temp;
    }

    temp.index = -1; // not found
    int total = inode->entry.size / sizeof(dir_entry);
    int blk;
    for (blk = 0; blk < inode->entry.blks; ++blk) {
        read_block(get_blk_for_inode(blk, &inode->entry), block_data);
        dir_entry *dir_p = (dir_entry *)block_data;

        while ((void *)(dir_p+1) < (void*)&block_data[BLK_SIZE]) {
            if (strcmp(dir_p->filename, filename) == 0) {
                temp.index = dir_p->inode;
                temp.entry = load_inode(temp.index);
                break;
            }
            dir_p++;
            total--;

            if (total == 0) {
                break;
            }
        }

        if (total == 0) {
            break;
        }
    }

    return temp;
}

iNode fsys_path_to_inode(const char *filename, Thread *thread)
{
    strcpy_to_kernel(thread, path, (char *)filename);
    int len = strlen(path);

    iNode temp;
    temp.index = 0; // root
    temp.entry = load_inode(temp.index);

    int l = 0;
    int r = l;

    while (l < len) {
        while (path[l] == '/') path[l++] = '\0';
        r = l;
        while (path[r] != '/' && r < len) r++;
        path[r] = '\0';

        char *name = &path[l];
        temp = search_dir(name, &temp);

        if (temp.index < 0) {
            // ERROR
            break;
        }

        l = r + 1;
    }

    return temp;
}

size_t fsys_read_by_filename(const char *filename, uint8_t *buf, off_t offset, size_t len, Thread *thread)
{
    if (len <= 0) {
        return 0;
    }

    iNode inode = fsys_path_to_inode(filename, thread);
    len = min(len, inode.entry.size - offset);

    off_t p = offset;
    size_t count = 0;

    while (len > 0) {
        uint32_t blk = p / BLK_SIZE;
        read_block(get_blk_for_inode(blk, &inode.entry), block_data);

        off_t start = p % BLK_SIZE;
        size_t l = min(len, BLK_SIZE - start);

        copy_from_kernel(thread, buf, &block_data[start], l);

        buf += l;
        p += l;
        len -= l;
        count += l;
    }

    return count;
}
