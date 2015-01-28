#ifndef __FSYS_H__
#define __FSYS_H__

#include "fs_types.h"
#include <unistd.h>

#define INODE_NUM 2048
#define BLK_NUM 2048
#define BLK_SIZE 1024

size_t dev_read(off_t offset, void *buf, size_t count);
size_t dev_write(off_t offset, void *buf, size_t count);

int get_free_inode();
int get_free_block();
void clear_inode_map(int index);
void clear_block_map(int index);
iNode_entry load_inode(int index);
void save_inode(int index, iNode_entry *inode);
size_t read_block(int index, void *buffer);
size_t write_block(int index, void *buffer);

int init_disk();
int make_sub_dir(const char *filename, int parent_inode);
int make_sub_file(const char* path, const char* filename, int parent_inode);

#endif /* __FSYS_H__ */
