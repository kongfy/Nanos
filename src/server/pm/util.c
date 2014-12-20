#include "kernel.h"
#include "server/pm.h"

void kill_thread(Thread *thread, int irq)
{
    Message m;
    PMMessage *msg = (PMMessage *)&m;
    msg->header.type = MSG_PM_KILL;
    msg->status = irq + 128;
    msg->pid = thread->pid;

    send(PM, &m);
    receive(PM, &m);

    return;
}
