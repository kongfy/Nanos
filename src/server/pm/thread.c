#include "kernel.h"
#include "server.h"

#include "hal.h"
#include "elf.h"


// 进程退出函数
void process_exit(void)
{
}

void create_progress(int file_name)
{
    assert(current->pid == PM);

    Thread *thread = create_thread();

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

    /* initialize the PCB, kernel stack, put the user process into ready queue */

    uint32_t *exit_addr = (uint32_t *)(thread->kstack + STK_SZ) - 1;
    *exit_addr = (uint32_t)process_exit;

    TrapFrame *tf = (TrapFrame *)(exit_addr) - 1;
    tf->eax = tf->ebx = tf->ecx = tf->edx = tf->esi = tf->edi = tf->ebp = tf->esp_ = 0;
    tf->cs = SELECTOR_KERNEL(SEG_KERNEL_CODE);
    tf->eip = (uint32_t)elf->entry;
    tf->eflags = 1 << 9; // 置IF位
    tf->err = tf->irq = 0;
    tf->gs = tf->fs = 0;
    tf->ds = tf->es = SELECTOR_KERNEL(SEG_KERNEL_DATA);
    thread->tf = tf;

    thread_ready(thread);
}

void create_first_process()
{
    create_progress(0);
    printf("first process created...\n");
}
