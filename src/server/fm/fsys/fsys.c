#include "fsys.h"
#include "charset.h"
#include "fsys_util.h"

static uint8_t block_data[BLK_SIZE];
static char s_buf[MAX_PATH_LEN];

static inline
int min(int a, int b)
{
    if (a < b) return a;
    return b;
}

size_t fsys_read_by_path(const char *path, uint8_t *buf, off_t offset, size_t len, Thread *thread, iNode *pwd)
{
    if (len <= 0) {
        return 0;
    }

    strcpy_to_kernel(thread, s_buf, (char *)path);
    iNode inode = fsys_path_to_inode(s_buf, pwd);
    if (inode.index < 0) {
        // not found
        return FILENOTFOUND;
    }

    if (inode.entry.type == DIRECTORY) {
        // is a directroy
        return ISDIRECTORY;
    }

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

int fsys_chdir(const char *path, Thread *thread)
{
    assert(thread->fm_struct);

    strcpy_to_kernel(thread, s_buf, (char *)path);
    iNode pwd = load_inode(thread->fm_struct->pwd);
    iNode inode = fsys_path_to_inode(s_buf, &pwd);
    if (inode.index < 0) {
        // not found
        return FILENOTFOUND;
    }

    if (inode.entry.type != DIRECTORY) {
        // is not a directroy
        return ISNDIRECTORY;
    }

    thread->fm_struct->pwd = inode.index;
    return 0;
}

int fsys_lsdir(const char *path, uint8_t *buf, Thread *thread)
{
    assert(thread->fm_struct);

    strcpy_to_kernel(thread, s_buf, (char *)path);
    iNode pwd = load_inode(thread->fm_struct->pwd);
    iNode inode = fsys_path_to_inode(s_buf, &pwd);
    if (inode.index < 0) {
        // not found
        return FILENOTFOUND;
    }

    if (inode.entry.type != DIRECTORY) {
        // is not a directroy
        return ISNDIRECTORY;
    }

    dir_entry *dest = (dir_entry *)buf;
    int total = inode.entry.size / sizeof(dir_entry);
    int blk;

    for (blk = 0; blk < inode.entry.blks; ++blk) {
        read_block(get_blk_for_inode(blk, &inode.entry), block_data);
        dir_entry *dir_p = (dir_entry *)block_data;

        while ((void *)(dir_p+1) < (void*)&block_data[BLK_SIZE]) {
            // copy first
            copy_from_kernel(thread, dest++, dir_p++, sizeof(dir_entry));
            total--;

            if (total == 0) break;
        }
        if (total == 0) break;
    }

    return inode.entry.size / sizeof(dir_entry);
}

int fsys_mkdir(const char *path, Thread *thread)
{
    assert(thread->fm_struct);

    strcpy_to_kernel(thread, s_buf, (char *)path);
    iNode pwd = load_inode(thread->fm_struct->pwd);
    iNode inode = fsys_path_to_inode(s_buf, &pwd);

    if (inode.index >= 0) {
        // already exist
        return -1;
    }

    // split parent dir if exist
    strcpy_to_kernel(thread, s_buf, (char *)path);
    iNode parent = load_inode(thread->fm_struct->pwd);
    int len = strlen(s_buf);
    int j = len - 1;
    while (is_blank_char(s_buf[j]) && j >= 0) s_buf[j--] = '\0';
    while (s_buf[j] == '/' && j >= 0) s_buf[j--] = '\0';
    while (s_buf[j] != '/' && j >= 0) j--;
    char *dirname = &s_buf[j + 1];

    if (j < 0) {
    } else if (j == 0) {
        parent = inode_for_root();
    } else {
        s_buf[j] = '\0';
        iNode pwd = load_inode(thread->fm_struct->pwd);
        parent = fsys_path_to_inode(s_buf, &pwd);
    }

    if (parent.index < 0) {
        return -2;
    }

    // check path name charset
    int dirname_len = strlen(dirname);
    int i = 0;
    for (i = 0; i < dirname_len; ++i) {
        if (!is_path_char(dirname[i])) return -3;
    }

    // mkdir
    inode = mkdir_to_parent(dirname, &parent);
    return inode.index > 0;
}
