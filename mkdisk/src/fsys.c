#include "fsys.h"
#include "fs_types.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BLOCKS_PER_BLOCK (BLK_SIZE / sizeof(int))
#define DIRECT_BLK 12
#define L1_BLK (DIRECT_BLK + BLOCKS_PER_BLOCK)
#define L2_BLK (L1_BLK + BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK)
#define L3_BLK (L2_BLK + BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK)

static uint8_t block[BLK_SIZE];

static inline
void clear_buffer()
{
    memset(block, 0, BLK_SIZE);
}

static
int add_blk_to_inode(iNode *inode)
{
    int blk = get_free_block();
    clear_buffer();
    write_block(blk, block);

    if (inode->blks + 1 < DIRECT_BLK) {
        inode->index[inode->blks + 1] = blk;
    } else if (inode->blks + 1 < L1_BLK) {
        if (inode->blks == DIRECT_BLK) {
            inode->index[12] = get_free_block();
        }

        int index = inode->blks + 1 - DIRECT_BLK;
        read_block(inode->index[12], block);
        *((int*)block + index) = blk;
        write_block(inode->index[12], block);
    } else if (inode->blks + 1 < L2_BLK) {
        if (inode->blks == L1_BLK) {
            inode->index[13] = get_free_block();
        }

        int l1_index = (inode->blks + 1 - L1_BLK) / BLOCKS_PER_BLOCK;
        int l2_index = (inode->blks + 1 - L1_BLK) % BLOCKS_PER_BLOCK;

        read_block(inode->index[13], block);
        if (l2_index == 0) {
            *((int*)block + l1_index) = get_free_block();
            write_block(inode->index[13], block);
        }

        int index = *((int *)block + l1_index);
        read_block(index, block);
        *((int*)block + l2_index) = get_free_block();
        write_block(index, block);
    } else if (inode->blks + 1 < L3_BLK) {
        if (inode->blks == L2_BLK) {
            inode->index[14] = get_free_block();
        }

        int l1_index = (inode->blks + 1 - L2_BLK) / (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK);
        int l2_index = (inode->blks + 1 - L2_BLK) % (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK) / BLOCKS_PER_BLOCK;
        int l3_index = (inode->blks + 1 - L2_BLK) % (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK) % BLOCKS_PER_BLOCK;

        read_block(inode->index[14], block);
        if (l2_index == 0 && l3_index == 0) {
            *((int*)block + l1_index) = get_free_block();
            write_block(inode->index[14], block);
        }

        int index = *((int *)block + l1_index);
        read_block(index, block);
        if (l3_index == 0) {
            *((int*)block + l2_index) = get_free_block();
        }
        write_block(index, block);

        index = *((int *)block + l2_index);
        read_block(index, block);
        *((int*)block + l3_index) = get_free_block();
        write_block(index, block);
    } else {
        printf("FATAL : file size limit exceeded.\n");
        exit(-1);
    }

    inode->blks ++;
    return blk;
}

static
int get_blk_for_inode(int blk, iNode *inode)
{
    if (blk < DIRECT_BLK) {
        return inode->index[blk];
    } else if (blk < L1_BLK) {
        int index = blk - DIRECT_BLK;
        read_block(inode->index[12], block);
        return *((int *)block + index);
    } else if (blk < L2_BLK) {
        int index = (blk - L1_BLK) / BLOCKS_PER_BLOCK;
        read_block(inode->index[13], block);
        read_block(*((int *)block + index), block);
        index = (blk - L1_BLK) % BLOCKS_PER_BLOCK;
        return *((int *)block + index);
    } else if (blk < L3_BLK) {
        int index = (blk - L2_BLK) / (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK);
        read_block(inode->index[14], block);
        read_block(*((int *)block + index), block);
        index = (blk - L2_BLK) % (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK) / BLOCKS_PER_BLOCK;
        read_block(*((int *)block + index), block);
        index = (blk - L2_BLK) % (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK) % BLOCKS_PER_BLOCK;
        return *((int *)block + index);
    } else {
        printf("FATAL : file size limit exceeded.\n");
        exit(-1);
    }
}

static
void add_to_parent(int parent_inode, int inode, const char *filename)
{
    iNode entry = load_inode(parent_inode);

    int blk = 0;
    int index = (entry.size / sizeof(dir_entry)) % (BLK_SIZE / sizeof(dir_entry));
    if (index == 0) {
        blk = add_blk_to_inode(&entry);
        read_block(blk, block);
    } else {
        blk = get_blk_for_inode(entry.blks, &entry);
        read_block(blk, block);
    }

    // this may cause small leak in size
    entry.size += sizeof(dir_entry);

    dir_entry *dir_p = ((dir_entry *)block) + index;
    strcpy(dir_p->filename, filename);
    dir_p->inode = inode;

    save_inode(parent_inode, &entry);
    write_block(blk, block);
}

int make_sub_dir(const char *filename, int parent_inode)
{
    int index = get_free_inode();

    iNode entry = load_inode(index);
    memset(&entry, 0, sizeof(iNode));
    entry.type = DIRECTORY;
    int blk = add_blk_to_inode(&entry);

    clear_buffer();
    dir_entry *dir_p = (dir_entry *)&block;
    strcpy(dir_p->filename, ".");
    dir_p->inode = index;
    dir_p ++;

    if (parent_inode < 0) {
        strcpy(dir_p->filename, "..");
        dir_p->inode = index;
    } else {
        strcpy(dir_p->filename, "..");
        dir_p->inode = parent_inode;
        add_to_parent(parent_inode, index, filename);
    }

    entry.size = sizeof(dir_entry) * 2;
    save_inode(index, &entry);
    write_block(blk, block);

    return index;
}

int make_sub_file(const char* path, const char* filename, int parent_inode)
{
    int index = get_free_inode();
    iNode entry = load_inode(index);

    add_to_parent(parent_inode, index, filename);
    memset(&entry, 0, sizeof(iNode));
    entry.size = PLAIN;

    int fd = open(path, O_RDONLY);
    int n = 0;
    clear_buffer();
    while ((n = read(fd, block, BLK_SIZE)) > 0) {
        int blk = add_blk_to_inode(&entry);
        write_block(blk, block);
        entry.size += n;
        clear_buffer();
    }

    save_inode(index, &entry);
    return index;
}
