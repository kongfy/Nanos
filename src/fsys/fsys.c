#include "kernel.h"
#include "kernel/pid.h"

#include "drivers/tty.h"
#include "drivers/ramdisk.h"

#include "fsys.h"

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define MAX_FILE 512

static fm_struct fms[MAX_PROCESS];
static OFTE sys_table[MAX_FILE];
static FI file_table[MAX_FILE];

// bit maps
static uint32_t stmap[MAX_FILE / 32];
static uint32_t ftmap[MAX_FILE / 32];

static inline
OFTE* get_free_sys_table()
{
    int index = 0;
    for (index = 0; index < MAX_FILE; ++index) {
        uint32_t mask = 1U << (index % 32);
        if (!(stmap[index >> 5] & mask)) {
            stmap[index >> 5] |= mask;
            return &sys_table[index];
        }
    }

    assert(0);
    return NULL;
}

static inline
FI* get_free_file_table()
{
    int index = 0;
    for (index = 0; index < MAX_FILE; ++index) {
        uint32_t mask = 1U << (index % 32);
        if (!(ftmap[index >> 5] & mask)) {
            ftmap[index >> 5] |= mask;
            return &file_table[index];
        }
    }

    assert(0);
    return NULL;
}

size_t ram_read(int file_name, uint8_t *buf, off_t offset, size_t len, Thread *thread)
{
    Device *dev = hal_get("ram");

    offset += file_name * NR_FILE_SIZE;
    return dev_read(dev, thread->pid, offset, buf, len);
}

int do_open(Thread *thread, int filename)
{
    return 0;
}

int do_close(Thread *thread, int fd)
{
    assert(thread->fm_struct);

    OFTE *ste = thread->fm_struct->fd[fd];
    if (!ste) {
        return -1; // error, fd is not opened.
    }

    FI *file = ste->file;
    if (-- file->_count == 0) {
        // close file
        int index = file - file_table;
        ftmap[index >> 5] &= ~(1U << index % 32);
    }

    if (-- ste->_count == 0) {
        // clean ste
        int index = ste - sys_table;
        stmap[index >> 5] &= ~(1U << index % 32);
    }

    thread->fm_struct->fd[fd] = NULL;

    return 0;
}

int do_read(Thread *thread, int fd, uint8_t *buf, int len)
{
    assert(thread->fm_struct);
    assert(thread->fm_struct->fd[fd]);

    OFTE *ste = thread->fm_struct->fd[fd];
    FI *file = ste->file;

    switch (file->type) {
    case Reg:
        break;
    case Dev: {
        return dev_read(file->dev, thread->pid, 0, buf, len);
    }
    }

    return 0;
}

int do_write(Thread *thread, int fd, uint8_t *buf, int len)
{
    assert(thread->fm_struct);
    assert(thread->fm_struct->fd[fd]);

    OFTE *ste = thread->fm_struct->fd[fd];
    FI *file = ste->file;

    switch (file->type) {
    case Reg:
        break;
    case Dev: {
        return dev_write(file->dev, thread->pid, 0, buf, len);
    }
    }

    return 0;
}

int do_lseek(Thread *Thread, int fd, int offset, int whence)
{
    return 0;
}

int do_dup(Thread *thread, int oldfd)
{
    assert(thread->fm_struct);
    assert(thread->fm_struct->fd[oldfd]);

    fm_struct *fm_struct = thread->fm_struct;

    int newfd = 0;
    for (; newfd < NR_FD; ++newfd) {
        if (fm_struct->fd[newfd] == NULL) {
            break;
        }
    }

    if (newfd >= NR_FD) {
        return -1;
    }

    fm_struct->fd[newfd] = fm_struct->fd[oldfd];
    ++ fm_struct->fd[newfd]->_count;

    return newfd;
}

int do_dup2(Thread *thread, int oldfd, int newfd)
{
    assert(thread->fm_struct);
    assert(thread->fm_struct->fd[oldfd]);

    if (newfd == oldfd) {
        return newfd;
    }

    fm_struct *fm_struct = thread->fm_struct;

    // close newfd if it was already opened
    if (fm_struct->fd[newfd]) {
        int flag = do_close(thread, newfd);
        if (flag < 0) {
            return -1;
        }
    }

    fm_struct->fd[newfd] = fm_struct->fd[oldfd];
    ++ fm_struct->fd[newfd]->_count;

    return newfd;
}

int do_pipe(Thread *Thread, int pipefd[2])
{
    return 0;
}

void init_fm_tty(Thread *thread, int tty)
{
    thread->fm_struct = &fms[thread->pid];

    char name[] = "tty*";
    Device *dev;
    name[3] = '0' + tty;
    dev = hal_get(name);

    FI *file = get_free_file_table();
    file->type = Dev;
    file->dev = dev;
    file->_count = 3;

    int fd;
    for (fd = 0; fd < 3; ++fd) {
        OFTE *ste = get_free_sys_table();
        ste->offset = 0;
        ste->file = file;
        ste->_count = 1;
        thread->fm_struct->fd[fd] = ste;
    }

    return;
}

void copy_fm(Thread *parent, Thread *child)
{
    assert(parent->fm_struct);

    child->fm_struct = &fms[child->pid];
    child->fm_struct = parent->fm_struct;

    int i = 0;
    for (; i < NR_FD; ++i) {
        OFTE *ste = child->fm_struct->fd[i];
        if (ste) {
            ++ ste->_count;
            ++ ste->file->_count;
        }
    }
}

void exit_fm(Thread *thread)
{
    int i = 0;
    for (; i < NR_FD; ++i) {
        do_close(thread, i);
    }
}
