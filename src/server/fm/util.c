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

// ONLY used by path_to_inode
static inline
void receive_fsys_block(int type, Message *m)
{
    FSYSMessage *msg = (FSYSMessage *)m;
    // ugly hack here...
    while (true) {
        receive(FSYSD, m);
        if (msg->header.type == type) break;

        Request_key key;
        key.type = REQ_FSYS;
        key.key.fsys.req_pid = msg->req_pid;
        reply(key, msg->ret);
    }
}

// blocking && kernel only
static
iNode path_to_inode(const char *filename, Thread *thread)
{
    assert(current->pid == FM);

    iNode temp;

    Message m;
    FSYSMessage *msg = (FSYSMessage *)&m;
    msg->header.type = MSG_FSYS_INODE_FOR_FILENAME;
    msg->req_pid = current->pid;
    msg->inode = &temp;
    msg->filename = filename;
    if (thread && thread->fm_struct) {
        msg->pwd = thread->fm_struct->pwd;
    } else {
        msg->pwd = 0;
    }

    send(FSYSD, &m);
    receive_fsys_block(MSG_FSYS_INODE_FOR_FILENAME, &m);

    return temp;
}

// ONLY used by pm to read off ELF header from disk
Request_key fs_read(const char *filename, uint8_t *buf, off_t offset, size_t len, Thread *thread, Thread *user)
{
    Message m;
    FSYSMessage *msg = (FSYSMessage *)&m;
    msg->header.type = MSG_FSYS_READ_BY_FILENAME;
    msg->req_pid = thread->pid;
    msg->filename = filename;
    msg->buf = buf;
    msg->offset = offset;
    msg->len = len;
    msg->pwd = user ? user->fm_struct->pwd : 0;

    send(FSYSD, &m);

    Request_key key;
    key.type = REQ_FSYS;
    key.key.fsys.req_pid = thread->pid;
    return key;
}

static
FI *open_file(iNode *inode)
{
    return NULL;
}

int do_open(Thread *thread, const char* filename)
{
    assert(thread->fm_struct);

    int fd;
    for (fd = 0; fd < NR_FD; ++fd) {
        if (!thread->fm_struct->fd[fd]) {
            iNode inode = path_to_inode(filename, thread);
            if (inode.index < 0) {
                return -1;
            }

            FI *file = open_file(&inode);
            OFTE *ste = get_free_sys_table();
            ste->offset = 0;
            ste->file = file;
            ste->_count = 1;
            thread->fm_struct->fd[fd] = ste;

            return fd;
        }
    }

    return -1;
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

Request_key do_read(Thread *thread, int fd, uint8_t *buf, int len)
{
    assert(thread->fm_struct);
    assert(thread->fm_struct->fd[fd]);

    OFTE *ste = thread->fm_struct->fd[fd];
    FI *file = ste->file;

    Request_key key;
    key.type = REQ_NULL;

    switch (file->type) {
    case Reg:
        break;
    case Dev: {
        dev_read(file->dev, thread->pid, 0, buf, len);
        key.type = REQ_DEV;
        key.key.dev.req_pid = thread->pid;
        key.key.dev.pid = file->dev->pid;
        key.key.dev.dev_id = file->dev->dev_id;
        break;
    }
    }

    return key;
}

Request_key do_write(Thread *thread, int fd, uint8_t *buf, int len)
{
    assert(thread->fm_struct);
    assert(thread->fm_struct->fd[fd]);

    OFTE *ste = thread->fm_struct->fd[fd];
    FI *file = ste->file;

    Request_key key;
    key.type = REQ_NULL;

    switch (file->type) {
    case Reg:
        break;
    case Dev: {
        dev_write(file->dev, thread->pid, 0, buf, len);
        key.type = REQ_DEV;
        key.key.dev.req_pid = thread->pid;
        key.key.dev.pid = file->dev->pid;
        key.key.dev.dev_id = file->dev->dev_id;
        break;
    }
    }

    return key;
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
    thread->fm_struct->pwd = 0; // hard code 0 for root path, BAD style

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
    *child->fm_struct = *parent->fm_struct;

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

Request_key do_chdir(Thread *thread, const char *path)
{
    Message m;
    FSYSMessage *msg = (FSYSMessage *)&m;
    msg->header.type = MSG_FSYS_CHDIR;
    msg->req_pid = thread->pid;
    msg->filename = path;

    send(FSYSD, &m);

    Request_key key;
    key.type = REQ_FSYS;
    key.key.fsys.req_pid = thread->pid;
    return key;
}

Request_key do_lsdir(Thread *thread, const char *path, uint8_t *buf)
{
    Message m;
    FSYSMessage *msg = (FSYSMessage *)&m;
    msg->header.type = MSG_FSYS_LSDIR;
    msg->req_pid = thread->pid;
    msg->filename = path;
    msg->buf = buf;

    send(FSYSD, &m);

    Request_key key;
    key.type = REQ_FSYS;
    key.key.fsys.req_pid = thread->pid;
    return key;
}

Request_key do_mkdir(Thread *thread, const char *path)
{
    Message m;
    FSYSMessage *msg = (FSYSMessage *)&m;
    msg->header.type = MSG_FSYS_MKDIR;
    msg->req_pid = thread->pid;
    msg->filename = path;
    send(FSYSD, &m);

    Request_key key;
    key.type = REQ_FSYS;
    key.key.fsys.req_pid = thread->pid;
    return key;

}

Request_key do_rmdir(Thread *thread, const char *path)
{
    Message m;
    FSYSMessage *msg = (FSYSMessage *)&m;
    msg->header.type = MSG_FSYS_RMDIR;
    msg->req_pid = thread->pid;
    msg->filename = path;
    send(FSYSD, &m);

    Request_key key;
    key.type = REQ_FSYS;
    key.key.fsys.req_pid = thread->pid;
    return key;

}

Request_key do_unlink(Thread *thread, const char *path)
{
    Message m;
    FSYSMessage *msg = (FSYSMessage *)&m;
    msg->header.type = MSG_FSYS_UNLINK;
    msg->req_pid = thread->pid;
    msg->filename = path;
    send(FSYSD, &m);

    Request_key key;
    key.type = REQ_FSYS;
    key.key.fsys.req_pid = thread->pid;
    return key;
}

Request_key do_stat(Thread *thread, const char *path, struct stat *buf)
{
    Message m;
    FSYSMessage *msg = (FSYSMessage *)&m;
    msg->header.type = MSG_FSYS_STAT;
    msg->req_pid = thread->pid;
    msg->filename = path;
    msg->buf = (uint8_t *)buf;
    send(FSYSD, &m);

    Request_key key;
    key.type = REQ_FSYS;
    key.key.fsys.req_pid = thread->pid;
    return key;
}

// call by FSYSD when remove a directory successfully
void pwd_evacuate(int pwd)
{
    int i = 0;
    for (; i < MAX_PROCESS; ++i) {
        if (fms[i].pwd == pwd) {
            fms[i].pwd = 0;
        }
    }
}
