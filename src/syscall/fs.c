#include "common.h"
#include "server/fm.h"

int sys_open(const char *filename)
{
    Message m;
    FMMessage *msg = (FMMessage *)&m;
    msg->header.type = MSG_FM_OPEN;
    msg->req_pid = current->pid;
    msg->filename = filename;

    send(FM, &m);
    receive(FM, &m);

    return msg->ret;
}

int sys_lseek(int fd, int offset, int whence)
{
    Message m;
    FMMessage *msg = (FMMessage *)&m;
    msg->header.type = MSG_FM_LSEEK;
    msg->req_pid = current->pid;
    msg->fd1 = fd;
    msg->offset = offset;
    msg->whence = whence;

    send(FM, &m);
    receive(FM, &m);

    return msg->ret;
}

int sys_close(int fd)
{
    Message m;
    FMMessage *msg = (FMMessage *)&m;
    msg->header.type = MSG_FM_CLOSE;
    msg->req_pid = current->pid;
    msg->fd1 = fd;

    send(FM, &m);
    receive(FM, &m);

    return msg->ret;
}

int sys_dup(int oldfd)
{
    Message m;
    FMMessage *msg = (FMMessage *)&m;
    msg->header.type = MSG_FM_DUP;
    msg->req_pid = current->pid;
    msg->fd1 = oldfd;

    send(FM, &m);
    receive(FM, &m);

    return msg->ret;
}

int sys_dup2(int oldfd, int newfd)
{
    Message m;
    FMMessage *msg = (FMMessage *)&m;
    msg->header.type = MSG_FM_DUP2;
    msg->req_pid = current->pid;
    msg->fd1 = oldfd;
    msg->fd2 = newfd;

    send(FM, &m);
    receive(FM, &m);

    return msg->ret;
}

int sys_pipe(int pipefd[2])
{
    Message m;
    FMMessage *msg = (FMMessage *)&m;
    msg->header.type = MSG_FM_PIPE;
    msg->req_pid = current->pid;
    msg->pipefd = (uint32_t)pipefd;

    send(FM, &m);
    receive(FM, &m);

    return msg->ret;
}

int sys_read(int fd, uint8_t *buf, int len)
{
    Message m;
    FMMessage *msg = (FMMessage *)&m;
    msg->header.type = MSG_FM_READ;
    msg->req_pid = current->pid;
    msg->fd1 = fd;
    msg->buf = (uint32_t)buf;
    msg->len = len;

    send(FM, &m);
    receive(FM, &m);

    return msg->ret;
}

int sys_write(int fd, uint8_t *buf, int len)
{
    Message m;
    FMMessage *msg = (FMMessage *)&m;
    msg->header.type = MSG_FM_WRITE;
    msg->req_pid = current->pid;
    msg->fd1 = fd;
    msg->buf = (uint32_t)buf;
    msg->len = len;

    send(FM, &m);
    receive(FM, &m);

    return msg->ret;
}

int sys_chdir(const char *path)
{
    Message m;
    FMMessage *msg = (FMMessage *)&m;
    msg->header.type = MSG_FM_CHDIR;
    msg->req_pid = current->pid;
    msg->filename = path;

    send(FM, &m);
    receive(FM, &m);

    return msg->ret;
}

int sys_lsdir(const char *path, uint8_t *buf)
{
    Message m;
    FMMessage *msg = (FMMessage *)&m;
    msg->header.type = MSG_FM_LSDIR;
    msg->req_pid = current->pid;
    msg->filename = path;
    msg->buf = (uint32_t)buf;

    send(FM, &m);
    receive(FM, &m);

    return msg->ret;
}
