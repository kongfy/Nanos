#ifndef FSYS_UTIL_H
#define FSYS_UTIL_H

#include "common.h"
#include "server.h"

// io.c
int get_free_inode();
int get_free_block();
void clear_inode_map(int index);
void clear_block_map(int index);
iNode load_inode(int index);
void save_inode(iNode *inode);
size_t read_block(int index, void *buffer);
size_t write_block(int index, void *buffer);

// util.c
int get_blk_for_inode(uint32_t blk, iNode_entry *inode);
int add_blk_to_inode(iNode_entry *inode);
iNode inode_for_root();
iNode fsys_path_to_inode(const char *path, iNode *pwd);
iNode mkdir_to_parent(char *dirname, iNode *parent);
int rm_from_parent(iNode *inode, iNode *parent);

// fsys.c
size_t fsys_read_by_path(const char *path, uint8_t *buf, off_t offset, size_t len, Thread *thread, iNode *pwd);
int fsys_chdir(const char *path, Thread *thread);
int fsys_lsdir(const char *path, uint8_t *buf, Thread *thread);
int fsys_mkdir(const char *path, Thread *thread);
int fsys_rmdir(const char *path, Thread *thread);
int fsys_unlink(const char *path, Thread *thread);
int fsys_stat(const char *path, struct stat *buf, Thread *thread);
int fsys_read(iNode *inode, uint8_t *buf, off_t offset, size_t len, Thread *thread);

#endif /* FSYS_UTIL_H */
