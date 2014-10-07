#include "kernel.h"
#include "server.h"

pid_t sys_fork(void)
{
    Message m;
    PMMessage *msg = (PMMessage *)&m;
    msg->header.type = MSG_PM_FORK;

    send(PM, &m);
    receive(PM, &m);

    return msg->ret;
}

int sys_exec(int filename, char *const argv[])
{
    Message m;
    PMMessage *msg = (PMMessage *)&m;
    msg->header.type = MSG_PM_EXEC;
    msg->filename = filename;
    msg->argv = (uint32_t)argv;

    send(PM, &m);
    receive(PM, &m);

    return msg->ret;
}

void sys_exit(int status)
{
    Message m;
    PMMessage *msg = (PMMessage *)&m;
    msg->header.type = MSG_PM_EXIT;
    msg->status = status;

    send(PM, &m);
    receive(PM, &m);

    return;
}
