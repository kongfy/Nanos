#include "kernel.h"
#include "server.h"
#include "memory.h"

#include "string.h"
#include "hal.h"
#include "elf.h"
#include "drivers/tty.h"


#define MAX_ARG_LEN 128
#define NR_ARG 32
#define ARG_BUF_SIZE (MAX_ARG_LEN * NR_ARG)

static char buf[ARG_BUF_SIZE];
static char * argv_buf[NR_ARG];


static
uint32_t init_with_elf(Thread *thread, int file_name)
{
    /* read 512 bytes starting from offset 0 from file "0" into buf */
    /* it contains the ELF header and program header table */
    uint8_t buf[512];

    Message m;

    FMMessage *msg = (FMMessage *)&m;
    m.type = MSG_FM_RD;
    msg->file_name = file_name;
    msg->offset = 0;
    msg->len = 512;
    msg->dest_addr = buf;
    msg->req_pid = current->pid;

    send(FM, &m);
    receive(FM, &m);
    assert(msg->ret == 512);

    // 通知MM为进程创建虚拟内存空间
    MMMessage *mmmsg = (MMMessage *)&m;
    m.type = MSG_MM_CREATE_VM;
    mmmsg->pid = thread->pid;

    send(MM, &m);
    receive(MM, &m);

    struct ELFHeader *elf;
    struct ProgramHeader *ph, *eph;

    elf = (struct ELFHeader*)buf;

    /* 把每个program segement依次读入内存 */
    ph = (struct ProgramHeader*)((char *)elf + elf->phoff);
    eph = ph + elf->phnum;
    for(; ph < eph; ph ++) {
        /* allocate pages starting from va, with memory size no less than ph->memsz */
        MMMessage *mmmsg = (MMMessage *)&m;

        m.type = MSG_MM_MMAP;
        mmmsg->pid = thread->pid;
        mmmsg->vaddr = ph->vaddr; /* virtual address */
        mmmsg->len = ph->memsz;
        mmmsg->read_write = (ph->flags & PF_W) ? 1 : 0;

        send(MM, &m);
        receive(MM, &m);

        /* read ph->filesz bytes starting from offset ph->off from file "0" into pa */
        FMMessage *fmmsg = (FMMessage *)&m;
        m.type = MSG_FM_RD;
        fmmsg->file_name = file_name;
        fmmsg->offset = ph->off;
        fmmsg->len = ph->filesz;
        fmmsg->req_pid = thread->pid;
        fmmsg->dest_addr = (void *)ph->vaddr;

        send(FM, &m);
        receive(FM, &m);

        assert(fmmsg->ret == ph->filesz);
    }

    // User stack, only one page (4 KB) memory by default
    // Please be careful!
    m.type = MSG_MM_MMAP;
    mmmsg->pid = thread->pid;
    mmmsg->vaddr = KOFFSET - PAGE_SIZE; /* virtual address */
    mmmsg->len = PAGE_SIZE;
    mmmsg->read_write = 1; // stack pages are always writeable.

    send(MM, &m);
    receive(MM, &m);

    return elf->entry;
}

void init_stack(Thread *thread, uint32_t entry, char **argv)
{
    /* initialize the PCB, user stack */

    void *top = &thread->kstack[STK_SZ];
    int offset = (void *)KOFFSET - top;

    // push argv into kernel stack, borrow kernel stack as buffer
    if (argv) {
        uint32_t total_len = 0;
        int32_t argc = 0;

        char **p = argv;
        while (*p) {
            argc++;
            total_len += strlen(*p) + 1;
            p++;
        }

        char *args = (char *)top - total_len;
        memcpy(args, buf, total_len);

        char **argvs = (char **)args;
        char *temp = (char *)top;
        int i = 0;
        for (i = argc - 1; i >= 0; --i) {
            temp -= strlen(argv[i]) + 1;
            argvs -= 1;
            *argvs = (void *)temp + offset;
        }

        char ***argv_position = (char ***)argvs - 1;
        *argv_position = (void *)argvs + offset;

        int32_t *argc_position = (int32_t *)argv_position - 1;
        *argc_position = argc;

        top = argc_position - 1; // leave the space for return EIP
    } else {
        char ***argv_position = (char ***)top - 1;
        *argv_position = NULL;

        int32_t *argc_position = (int32_t *)argv_position - 1;
        *argc_position = 0;

        top = argc_position - 1; // leave the space for return EIP
    }

    // copy to user space
    size_t len = (void *)&thread->kstack[STK_SZ] - top;
    uint32_t esp = KOFFSET - len;
    copy_from_kernel(thread, (void *)esp, top, len);

    TrapFrame *tf = (TrapFrame *)&thread->kstack[STK_SZ] - 1;
    tf->eax = tf->ebx = tf->ecx = tf->edx = tf->esi = tf->edi = tf->ebp = tf->esp_ = 0;
    tf->cs = SELECTOR_USER(SEG_USER_CODE);
    tf->eip = entry;
    tf->eflags = 1 << 9; // 置IF位
    tf->err = tf->irq = 0;
    tf->gs = tf->fs = 0;
    tf->ds = tf->es = tf->ss = SELECTOR_USER(SEG_USER_DATA);
    tf->esp = esp;
    thread->tf = tf;
}

void create_shells()
{
    int i;
    for (i = 1; i <= NR_TTY; ++i) {
        Thread *thread = create_thread();
        uint32_t entry = init_with_elf(thread, 2);
        init_stack(thread, entry, NULL);

        // initialize with fm
        Message m;
        FMMessage *msg = (FMMessage *)&m;
        m.type = MSG_FM_INIT;
        msg->req_pid = thread->pid;
        msg->tty = i;

        send(FM, &m);
        receive(FM, &m);

        thread_ready(thread);
    }
}

static
void clone_tcb(Thread *parent, Thread *child)
{
    memcpy(child->kstack, parent->kstack, STK_SZ);

    int32_t offset = child->kstack - parent->kstack;

    uint32_t *p = (uint32_t *)parent->tf->ebp;
    int i = 0;
    for (i = 0 ; i < 4; ++i) {
        p = (uint32_t *)*p;
    }

    // do not block child precess!!!
    TrapFrame *tf = (TrapFrame *)*(p + 2);
    child->tf = (TrapFrame *)((uint32_t)tf + offset);
    child->tf->eax = 0; // fork() return value for child process
    child->tf->esp_ += offset;

    return;
}

pid_t do_fork(Thread *thread)
{
    assert(current->pid == PM);

    Thread *child = create_thread();
    clone_tcb(thread, child);

    // fork memory space for child process
    Message m;
    MMMessage *msg = (MMMessage *)&m;
    m.type = MSG_MM_FORK;
    msg->pid = thread->pid;
    msg->child_pid = child->pid;
    send(MM, &m);
    receive(MM, &m);

    // copy with fm
    FMMessage *fmsg = (FMMessage *)&m;
    m.type = MSG_FM_COPY;
    fmsg->req_pid = thread->pid;
    fmsg->child_pid = child->pid;
    send(FM, &m);
    receive(FM, &m);

    thread_ready(child);

    return child->pid;
}

static
void mm_exit_mm(Thread *thread)
{
    Message m;
    MMMessage *msg = (MMMessage *)&m;
    msg->header.type = MSG_MM_REVOKE_VM;
    msg->pid = thread->pid;

    send(MM, &m);
    receive(MM, &m);
}

int do_exec(Thread *thread, int filename, char *argv[])
{
    // copy the argv to kernel buffer
    if (argv) {
        char **p = argv_buf;
        char **q = argv;
        char *arg = buf;

        while (true) {
            char *str;
            copy_to_kernel(thread, &str, q++, sizeof str);

            if (!str) {
                break;
            }

            int len = strcpy_to_kernel(thread, arg, str);
            *(p++) = arg;
            arg += len;
        };

        *p = NULL;
    }

    // clean up memeory
    mm_exit_mm(thread);

    // clean up old PCB
    thread->status = Ready;
    thread->lock_count = 0;
    init_sem(&thread->msg_sem, 0);
    thread->msg_head = thread->msg_tail = 0;

    uint32_t entry = init_with_elf(thread, filename);
    init_stack(thread, entry, argv_buf);

    thread_ready(thread);

    return 0;
}

typedef struct
{
    list_head queue;
    pid_t pid;
    Thread *thread;
} WaitPair;

#define MAX_PAIR 256

static list_head waitq, freeq;
static WaitPair pairs[MAX_PAIR];

void init_waitpid_structure()
{
    INIT_LIST_HEAD(&waitq);
    INIT_LIST_HEAD(&freeq);

    WaitPair *p = pairs;
    while (p < &pairs[MAX_PAIR]) {
        list_add_tail(&p->queue, &freeq);
        p++;
    }
}

void do_waitpid(Thread *thread, pid_t pid)
{
    assert(!list_empty(&freeq));

    // do not need lock here, because there is only one thread(PM).
    WaitPair* pair = list_entry(freeq.next, WaitPair, queue);
    pair->pid = pid;
    pair->thread = thread;

    list_del(&pair->queue);
    list_add_tail(&pair->queue, &waitq);
}

static void wait_notify(pid_t pid, int status)
{
    assert(PM == current->pid);

    WaitPair *pos, *n;
    list_for_each_entry_safe(pos, n, &waitq, queue) {
        if (pos->pid == pid) {
            Message m;
            PMMessage *msg = (PMMessage *)&m;
            msg->ret = status;
            send(pos->thread->pid, &m);

            list_del(&pos->queue);
            list_add_tail(&pos->queue, &freeq);
        }
    }
}

void do_exit(Thread *thread, int status)
{
    pid_t pid = thread->pid;

    // clean up memeory
    mm_exit_mm(thread);

    // exit with fm
    Message m;
    FMMessage *msg = (FMMessage *)&m;
    m.type = MSG_FM_EXIT;
    msg->req_pid = thread->pid;
    send(FM, &m);
    receive(FM, &m);

    // clean up PCB
    thread_exit(thread);

    wait_notify(pid, status);

    return;
}
