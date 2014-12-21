#include "kernel.h"
#include "server/pm.h"

void kill_thread(Thread *thread, int irq)
{
    Message m;
    PMMessage *msg = (PMMessage *)&m;
    msg->header.type = MSG_PM_KILL;
    msg->status = irq + 128;
    msg->pid = thread->pid;

    char *reason;
    switch (irq) {
    case 0:
        reason = "Divide Error";
        break;
    case 1:
        reason = "Debug Exception";
        break;
    case 2:
        reason = "Non-maskable Hardware Interrupt";
        break;
    case 3:
        reason = "INT 3";
        break;
    case 4:
        reason = "INTO Instruction Detected Overflow";
        break;
    case 5:
        reason = "BOUND Instruction Detected Overflow";
        break;
    case 6:
        reason = "Invalid Instruction Opcode";
        break;
    case 7:
        reason = "No Coprocessor";
        break;
    case 8:
        reason = "Double Fault";
        break;
    case 9:
        reason = "Coprocessor Segment Overrun";
        break;
    case 10:
        reason = "Invalid Task State Segment";
        break;
    case 11:
        reason = "Segment Not Present";
        break;
    case 12:
        reason = "Stack Fault";
        break;
    case 13:
        reason = "General Protection Fault";
        break;
    case 14:
        reason = "Page Fault";
        break;
    default:
        reason = "Unknown";
    }

    printf("KILL %d : %s\n", thread->pid, reason);

    send(PM, &m);
    receive(PM, &m);

    return;
}
