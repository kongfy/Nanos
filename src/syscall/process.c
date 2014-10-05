#include "kernel.h"
#include "server.h"

pid_t fork(void)
{
    Message m;
    PMMessage *msg = (PMMessage *)&m;
    msg->header.type = MSG_PM_FORK;

    send(PM, &m);
    receive(PM, &m);

    return msg->ret;
}
