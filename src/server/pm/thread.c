#include "kernel.h"
#include "server.h"

#include "string.h"
#include "hal.h"
#include "elf.h"


#define MAX_ARG_LEN 128
#define NR_ARG 32
#define ARG_BUF_SIZE (MAX_ARG_LEN * NR_ARG)

static char buf[ARG_BUF_SIZE];
static char * argv_buf[NR_ARG];


static
uint32_t init_process_with_elf(Thread *thread, int file_name)
{
    /* read 512 bytes starting from offset 0 from file "0" into buf */
    /* it contains the ELF header and program header table */
    uint8_t buf[512];

    Message m;

    FMMessage *msg = (FMMessage *)&m;
    m.type = MSG_FM_RW;
    msg->file_name = file_name;
    msg->buf = buf;
    msg->offset = 0;
    msg->len = 512;
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
    unsigned char *pa, *i;

    elf = (struct ELFHeader*)buf;

    /* 把每个program segement依次读入内存 */
    ph = (struct ProgramHeader*)((char *)elf + elf->phoff);
    eph = ph + elf->phnum;
    for(; ph < eph; ph ++) {
        /* allocate pages starting from va, with memory size no less than ph->memsz */

        MMMessage *mmmsg = (MMMessage *)&m;

        m.type = MSG_MM_ALLOC_PAGES;
        mmmsg->pid = thread->pid;
        mmmsg->vaddr = ph->vaddr; /* virtual address */
        mmmsg->memsz = ph->memsz;

        send(MM, &m);
        receive(MM, &m);

        pa = (unsigned char *)mmmsg->paddr;

        /* read ph->filesz bytes starting from offset ph->off from file "0" into pa */
        FMMessage *fmmsg = (FMMessage *)&m;
        m.type = MSG_FM_RW;
        fmmsg->file_name = file_name;
        fmmsg->buf = pa;
        fmmsg->offset = ph->off;
        fmmsg->len = ph->filesz;
        fmmsg->req_pid = current->pid;

        send(FM, &m);
        receive(FM, &m);

        assert(fmmsg->ret == ph->filesz);

        for (i = pa + ph->filesz; i < pa + ph->memsz; *i ++ = 0);
    }

    return elf->entry;
}

void init_stack(Thread *thread, uint32_t entry, char **argv)
{
    /* initialize the PCB, kernel stack */

    void *top = &thread->kstack[STK_SZ];

    // push argv into stack
    if (argv) {
        uint32_t total_len = 0;
        int32_t argc = 0;

        char **p = argv_buf;
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
            temp -= strlen(argv_buf[i]) + 1;
            argvs -= 1;
            *argvs = temp;
        }

        char ***argv_position = (char ***)argvs - 1;
        *argv_position = argvs;

        int32_t *argc_position = (int32_t *)argv_position - 1;
        *argc_position = argc;

        top = argc_position - 1; // leave the space for return EIP
    }

    TrapFrame *tf = (TrapFrame *)top - 1;
    tf->eax = tf->ebx = tf->ecx = tf->edx = tf->esi = tf->edi = tf->ebp = tf->esp_ = 0;
    tf->cs = SELECTOR_KERNEL(SEG_KERNEL_CODE);
    tf->eip = entry;
    tf->eflags = 1 << 9; // 置IF位
    tf->err = tf->irq = 0;
    tf->gs = tf->fs = 0;
    tf->ds = tf->es = SELECTOR_KERNEL(SEG_KERNEL_DATA);
    thread->tf = tf;
}

void create_first_process()
{
    Thread *thread = create_thread();
    uint32_t entry = init_process_with_elf(thread, 0);
    init_stack(thread, entry, NULL);
    thread_ready(thread);
}

static
void clone_pcb(Thread *parent, Thread *child)
{
    memcpy(child->kstack, parent->kstack, STK_SZ);

    int32_t offset = child->kstack - parent->kstack;

    uint32_t *p = (uint32_t *)parent->tf->ebp;
    int i = 0;
    for (i = 0 ; i < 4; ++i) {
        p = (uint32_t *)*p;
    }

    TrapFrame *tf = (TrapFrame *)*(p + 2);
    child->tf = (TrapFrame *)((uint32_t)tf + offset);
    child->tf->eax = 0;
    child->tf->esp_ += offset;

    uint32_t *bp = (uint32_t *)&child->tf->ebp;
    while (*bp) {
        *bp += offset;
        bp = (uint32_t *)*bp;
    }

    return;
}

pid_t fork_process(Thread *thread)
{
    assert(current->pid == PM);

    Thread *child = create_thread();
    clone_pcb(thread, child);


    // fork memory space for child process
    Message m;
    MMMessage *msg = (MMMessage *)&m;
    m.type = MSG_MM_FORK;
    msg->pid = thread->pid;
    msg->child_pid = child->pid;

    send(MM, &m);
    receive(MM, &m);

    thread_ready(child);

    return child->pid;
}

static
void revoke_vm(Thread *thread)
{
    Message m;
    MMMessage *msg = (MMMessage *)&m;
    msg->header.type = MSG_MM_REVOKE_VM;
    msg->pid = thread->pid;

    send(MM, &m);
    receive(MM, &m);
}

int exec_process(Thread *thread, int filename, char *argv[])
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
    revoke_vm(thread);

    // clean up old PCB
    thread->status = Ready;
    thread->lock_count = 0;
    init_sem(&thread->msg_sem, 0);
    thread->msg_head = thread->msg_tail = 0;

    uint32_t entry = init_process_with_elf(thread, filename);
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

void waitpid_process(Thread *thread, pid_t pid)
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

void exit_process(Thread *thread, int status)
{
    pid_t pid = thread->pid;

    // clean up memeory
    revoke_vm(thread);
    // clean up PCB
    thread_exit(thread);

    wait_notify(pid, status);

    return;
}
