#include "kernel.h"
#include "server.h"

#include "drivers/time.h"

pid_t sys_fork(void)
{
    Message m;
    PMMessage *msg = (PMMessage *)&m;
    msg->header.type = MSG_PM_FORK;

    send(PM, &m);
    receive(PM, &m);

    return msg->ret;
}

int32_t sys_exec(const char *filename, char *const argv[])
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

int32_t sys_waitpid(pid_t pid)
{
    Message m;
    PMMessage *msg = (PMMessage *)&m;
    msg->header.type = MSG_PM_WAITPID;
    msg->pid = pid;

    send(PM, &m);
    receive(PM, &m);

    return msg->ret;
}

uint32_t sys_sleep(uint32_t seconds)
{
    Message m;
    TIMEMessage *msg = (TIMEMessage *)&m;
    msg->header.type = MSG_TIME_ALARM;
    msg->seconds = seconds;

    send(TIME, &m);
    receive(TIME, &m);

    return msg->seconds;
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
