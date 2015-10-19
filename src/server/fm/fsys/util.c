#include "kernel.h"
#include "charset.h"
#include "fsys_util.h"
#include "string.h"

#define BLOCKS_PER_BLOCK (BLK_SIZE / sizeof(int))
#define DIRECT_BLK 12
#define L1_BLK (DIRECT_BLK + BLOCKS_PER_BLOCK)
#define L2_BLK (L1_BLK + BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK)
#define L3_BLK (L2_BLK + BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK)

static uint8_t block_data[BLK_SIZE], block_index[BLK_SIZE];
static uint8_t block_index_l2[BLK_SIZE], block_index_l3[BLK_SIZE]; // used in erase_file
static char s_buf[MAX_PATH_LEN];

static inline
int min(int a, int b)
{
    return a < b ? a : b;
}

int get_blk_for_inode(uint32_t blk, iNode_entry *inode)
{
    if (blk < DIRECT_BLK) {
        return inode->index[blk];
    } else if (blk < L1_BLK) {
        int index = blk - DIRECT_BLK;
        read_block(inode->index[DIRECT_BLK], block_index);
        return *((int *)block_index + index);
    } else if (blk < L2_BLK) {
        int index = (blk - L1_BLK) / BLOCKS_PER_BLOCK;
        read_block(inode->index[DIRECT_BLK + 1], block_index);
        read_block(*((int *)block_index + index), block_index);
        index = (blk - L1_BLK) % BLOCKS_PER_BLOCK;
        return *((int *)block_index + index);
    } else if (blk < L3_BLK) {
        int index = (blk - L2_BLK) / (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK);
        read_block(inode->index[DIRECT_BLK + 2], block_index);
        read_block(*((int *)block_index + index), block_index);
        index = (blk - L2_BLK) % (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK) / BLOCKS_PER_BLOCK;
        read_block(*((int *)block_index + index), block_index);
        index = (blk - L2_BLK) % (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK) % BLOCKS_PER_BLOCK;
        return *((int *)block_index + index);
    } else {
        return -1;
    }
}

int add_blk_to_inode(iNode_entry *inode)
{
    int blk = get_free_block();
    memset(block_data, 0, BLK_SIZE);
    write_block(blk, block_data);

    if (inode->blks < DIRECT_BLK) {
        inode->index[inode->blks] = blk;
    } else if (inode->blks < L1_BLK) {
        if (inode->blks == DIRECT_BLK) {
            inode->index[DIRECT_BLK] = get_free_block();
        }

        int index = inode->blks - DIRECT_BLK;
        read_block(inode->index[DIRECT_BLK], block_data);
        *((int*)block_data + index) = blk;
        write_block(inode->index[DIRECT_BLK], block_data);
    } else if (inode->blks < L2_BLK) {
        if (inode->blks == L1_BLK) {
            inode->index[DIRECT_BLK + 1] = get_free_block();
        }

        int l1_index = (inode->blks - L1_BLK) / BLOCKS_PER_BLOCK;
        int l2_index = (inode->blks - L1_BLK) % BLOCKS_PER_BLOCK;

        read_block(inode->index[DIRECT_BLK + 1], block_data);
        if (l2_index == 0) {
            *((int*)block_data + l1_index) = get_free_block();
            write_block(inode->index[DIRECT_BLK + 1], block_data);
        }

        int index = *((int *)block_data + l1_index);
        read_block(index, block_data);
        *((int*)block_data + l2_index) = get_free_block();
        write_block(index, block_data);
    } else if (inode->blks < L3_BLK) {
        if (inode->blks == L2_BLK) {
            inode->index[DIRECT_BLK + 2] = get_free_block();
        }

        int l1_index = (inode->blks - L2_BLK) / (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK);
        int l2_index = (inode->blks - L2_BLK) % (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK) / BLOCKS_PER_BLOCK;
        int l3_index = (inode->blks - L2_BLK) % (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK) % BLOCKS_PER_BLOCK;

        read_block(inode->index[DIRECT_BLK + 2], block_data);
        if (l2_index == 0 && l3_index == 0) {
            *((int*)block_data + l1_index) = get_free_block();
            write_block(inode->index[DIRECT_BLK + 2], block_data);
        }

        int index = *((int *)block_data + l1_index);
        read_block(index, block_data);
        if (l3_index == 0) {
            *((int*)block_data + l2_index) = get_free_block();
        }
        write_block(index, block_data);

        index = *((int *)block_data + l2_index);
        read_block(index, block_data);
        *((int*)block_data + l3_index) = get_free_block();
        write_block(index, block_data);
    } else {
        panic("FATAL : file size limit exceeded.\n");
    }

    inode->blks ++;
    return blk;
}

void erase_file(iNode *inode)
{
    int blks = inode->entry.blks;

    // clear direct blocks
    int d_index;
    int d_t = min(DIRECT_BLK, blks);

    for (d_index = 0; d_index < d_t; ++d_index) {
        clear_block_map(inode->entry.index[d_index]);
    }

    if (blks > DIRECT_BLK) {
        // clear L1
        int l1_index;
        int l1_t = min(L1_BLK, blks) - DIRECT_BLK;

        read_block(inode->entry.index[DIRECT_BLK], block_index);
        for (l1_index = 0; l1_index < l1_t; ++l1_index) {
            clear_block_map(*((int *)block_index + l1_index));
        }

        clear_block_map(inode->entry.index[DIRECT_BLK]);
    }

    if (blks > L1_BLK) {
        // clear L2
        int l1_index;
        int l1_t = (min(L2_BLK, blks) - L1_BLK - 1) / BLOCKS_PER_BLOCK + 1;

        read_block(inode->entry.index[DIRECT_BLK + 1], block_index);
        for (l1_index = 0; l1_index < l1_t; ++l1_index) {
            int l2_index;
            int l2_t = min(L1_BLK
                           + (l1_index + 1) * BLOCKS_PER_BLOCK, blks)
                - (L1_BLK
                   + l1_index * BLOCKS_PER_BLOCK);

            read_block(*((int *)block_index + l1_index), block_index_l2);
            for (l2_index = 0; l2_index < l2_t; ++l2_index) {
                clear_block_map(*((int *)block_index_l2 + l2_index));
            }

            clear_block_map(*((int *)block_index + l1_index));
        }
        clear_block_map(inode->entry.index[DIRECT_BLK + 1]);
    }

    if (blks > L2_BLK) {
        // clear L3
        int l1_index;
        int l1_t = (min(L3_BLK, blks) - L2_BLK - 1)
            / (BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK) + 1;

        read_block(inode->entry.index[DIRECT_BLK + 2], block_index);
        for (l1_index = 0; l1_index < l1_t; ++l1_index) {
            int l2_index;
            int l2_t = min(L2_BLK
                           + (l1_index + 1) * BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK, blks)
                - (L2_BLK
                   + l1_index * BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK);

            read_block(*((int *)block_index + l1_index), block_index_l2);
            for (l2_index = 0; l2_index < l2_t; ++l2_index) {
                int l3_index;
                int l3_t = min(L2_BLK
                               + l1_index * BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK
                               + (l2_index + 1) * BLOCKS_PER_BLOCK, blks)
                    - (L2_BLK
                       + l1_index * BLOCKS_PER_BLOCK * BLOCKS_PER_BLOCK
                       + l2_index * BLOCKS_PER_BLOCK);

                read_block(*((int *)block_index_l2 + l2_index), block_index_l3);
                for (l3_index = 0; l3_index < l3_t; ++l3_index) {
                    clear_block_map(*((int *)block_index_l3 + l3_index));
                }

                clear_block_map(*((int *)block_index_l2 + l2_index));
            }

            clear_block_map(*((int *)block_index + l1_index));
        }
        clear_block_map(inode->entry.index[DIRECT_BLK + 2]);
    }

    clear_inode_map(inode->index);
}

iNode inode_for_root()
{
    iNode temp = load_inode(0); // root
    return temp;
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
            if (total == 0) {
                break;
            }

            if (strcmp(dir_p->filename, filename) == 0) {
                temp = load_inode(dir_p->inode);
                return temp;
            }
            dir_p++;
            total--;
        }
    }

    return temp;
}

// char *path must point to kernel space
iNode fsys_path_to_inode(const char *path, iNode *pwd)
{
    strcpy(s_buf, path);

    int len = strlen(s_buf);
    char *p = s_buf;

    // skip invalid path
    if (!check_path_charset(p)) {
        iNode temp;
        temp.index = -1;
        return temp;
    }

    iNode temp;
    if (pwd && !(len > 0 && p[0] == '/')) {
        temp = *pwd;
    } else {
        temp = inode_for_root();
    }

    int l = 0;
    int r = l;

    while (l < len) {
        while (p[l] == '/') p[l++] = '\0';
        r = l;
        while (p[r] != '/' && r < len) r++;
        p[r] = '\0';

        char *name = &p[l];
        if (strlen(name) == 0) break;

        temp = search_dir(name, &temp);

        if (temp.index < 0) {
            // ERROR
            break;
        }

        l = r + 1;
    }

    return temp;
}

void add_to_parent(iNode *parent, iNode *inode, char *filename)
{
    int blk = 0;
    int index = (parent->entry.size / sizeof(dir_entry)) % (BLK_SIZE / sizeof(dir_entry));
    if (index == 0) {
        blk = add_blk_to_inode(&parent->entry);
        read_block(blk, block_data);
    } else {
        blk = get_blk_for_inode(parent->entry.blks - 1, &parent->entry);
        read_block(blk, block_data);
    }

    dir_entry *dir_p = (dir_entry *)block_data + index;
    strcpy(dir_p->filename, filename);
    dir_p->inode = inode->index;

    // this may cause small leak in size
    parent->entry.size += sizeof(dir_entry);

    save_inode(parent);
    write_block(blk, block_data);
}

iNode mkdir_to_parent(char *dirname, iNode *parent)
{
    assert(parent);
    assert(parent->index >= 0);

    iNode inode;
    inode.index = get_free_inode();
    memset(&inode.entry, 0, sizeof(iNode_entry));
    inode.entry.type = DIRECTORY;

    int blk = add_blk_to_inode(&inode.entry);
    dir_entry *dir_p = (dir_entry *)&block_data;

    strcpy(dir_p->filename, ".");
    dir_p->inode = inode.index;
    dir_p ++;

    strcpy(dir_p->filename, "..");
    dir_p->inode = parent->index;


    inode.entry.size = sizeof(dir_entry) * 2;
    save_inode(&inode);
    write_block(blk, block_data);

    add_to_parent(parent, &inode, dirname);

    return inode;
}

int rm_from_parent(iNode *inode, iNode *parent)
{
    assert(parent);
    assert(parent->index >= 0);
    assert(parent->entry.type == DIRECTORY);

    // erase from parent dir
    int total = parent->entry.size / sizeof(dir_entry);
    int blk;
    for (blk = 0; blk < parent->entry.blks; ++blk) {
        read_block(get_blk_for_inode(blk, &parent->entry), block_data);
        dir_entry *dir_p = (dir_entry *)block_data;

        while ((void *)(dir_p+1) < (void*)&block_data[BLK_SIZE]) {
            if (total == 0) {
                break;
            }

            if (dir_p->inode == inode->index) {
                if (total != 1) {
                    // swap
                    dir_entry x = *dir_p;
                    int last_blk = get_blk_for_inode(parent->entry.blks - 1, &parent->entry);
                    read_block(last_blk, block_data);

                    int last = (parent->entry.size / sizeof(dir_entry))
                        % (BLK_SIZE / sizeof(dir_entry)) - 1;
                    dir_entry y = ((dir_entry *)block_data)[last];

                    ((dir_entry *)block_data)[last] = x;
                    write_block(last_blk, block_data);

                    int origin_blk = get_blk_for_inode(blk, &parent->entry);
                    read_block(origin_blk, block_data);
                    *dir_p = y;
                    write_block(origin_blk, block_data);
                }

                // erase last file
                parent->entry.size -= sizeof(dir_entry);
                if (0 == (parent->entry.size / sizeof(dir_entry)) % (BLK_SIZE / sizeof(dir_entry))) {
                    clear_block_map(get_blk_for_inode(parent->entry.blks - 1, &parent->entry));
                    parent->entry.blks--;
                }
                save_inode(parent);

                // erase file
                erase_file(inode);

                return 0;
            }

            dir_p++;
            total--;
        }
    }

    assert(0); // should not reach here. file not found
    return 0;
}
