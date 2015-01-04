#include "common.h"
#include "server/fm.h"

int sys_open(int filename)
{
    Message m;
    FMMessage *msg = (FMMessage *)&m;
    msg->header.type = MSG_FM_OPEN;
    msg->file_name = filename;

    send(FM, &m);
    receive(FM, &m);

    return msg->ret;
}

int sys_lseek(int fd, int offset, int whence)
{
    Message m;
    FMMessage *msg = (FMMessage *)&m;
    msg->header.type = MSG_FM_LSEEK;
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
    msg->fd1 = fd;
    msg->buf = (uint32_t)buf;
    msg->len = len;

    send(FM, &m);
    receive(FM, &m);

    return msg->ret;
}
