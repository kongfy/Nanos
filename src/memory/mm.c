/*
 * vm.c

 *
 *  Created on: 2014-9-17
 *      Author: kongfy
 */


#include "kernel.h"
#include "kernel/pid.h"

#include "memory.h"
#include "string.h"

// 创建一个存放页表的空间
#define TOTAL_MAX_PT (32 * ((PHY_MEM - KMEM) / PAGE_SIZE / NR_PTE))

// 用户进程虚拟内存
static mm_struct mms[MAX_PROCESS];                                       // 用户进程 mm_struct
static PDE pdir[MAX_PROCESS][NR_PDE] align_to_page;                      // 用户进程 page directory
static PTE ptables[TOTAL_MAX_PT][NR_PTE] align_to_page;                  // 用户进程 page tables
static unsigned char ptmap[TOTAL_MAX_PT / 8];                            // page table bit map

// 得到一页页表的第一个页表项，注意页表项是连续使用1024个的
static inline
PTE* get_free_ptable()
{
    int index = 0;
    for (index = 0; index < TOTAL_MAX_PT; ++index) {
        unsigned char mask = 1 << (index % 8);
        if (!(ptmap[index / 8] & mask)) {
            ptmap[index / 8] |= mask;
            return ptables[index];
        }
    }

    assert(0); // page table用光了
    return NULL;
}

void allocate_mm(Thread *thread)
{
    PDE *phy_pdir = (PDE *)va_to_pa(pdir[thread->pid]);

    uint32_t pdir_idx;

    for (pdir_idx = 0; pdir_idx < NR_PDE; pdir_idx ++) {
        make_invalid_pde(&phy_pdir[pdir_idx]);
    }

    // 高位内存映射到内核空间
    PDE *kpdir = (PDE *)va_to_pa(get_kpdir());
    for (pdir_idx = 0; pdir_idx < KMEM / PD_SIZE; pdir_idx ++) {
        PDE *kpde = &kpdir[pdir_idx + KOFFSET / PD_SIZE];
        uint32_t ptable = kpde->page_frame << 12;
        make_pde(&phy_pdir[pdir_idx + KOFFSET / PD_SIZE], (void*)ptable);
    }

    mms[thread->pid].pgd = phy_pdir;
    thread->mm_struct = &mms[thread->pid];
}

void copy_mm(Thread *parent, Thread *child)
{
    PDE *parent_pdir = (PDE *)va_to_pa(pdir[parent->pid]);
    PDE *child_pdir = (PDE *)va_to_pa(pdir[child->pid]);

    uint32_t pdir_idx, ptable_idx;

    for (pdir_idx = 0; pdir_idx < KOFFSET / PD_SIZE; pdir_idx ++) {
        PDE *parent_pde = &parent_pdir[pdir_idx];
        PDE *child_pde = &child_pdir[pdir_idx];

        if (parent_pde->val) {
            PTE *child_ptable = (PTE*)va_to_pa(get_free_ptable());
            make_pde(child_pde, child_ptable);

            PTE *parent_ptable = (PTE*)(parent_pde->page_frame << 12);
            for (ptable_idx = 0; ptable_idx < NR_PTE; ptable_idx ++) {
                PTE *parent_pte = &parent_ptable[ptable_idx];
                PTE *child_pte = &child_ptable[ptable_idx];

                if (parent_pte->present) {
                    // alloc one page for child process
                    uint32_t pa = get_free_page();
                    make_pte(child_pte, (void *)pa);

                    // copy physical memory
                    memcpy((void *)pa, (void *)(parent_pte->page_frame << 12), PAGE_SIZE);
                }
            }
        }
    }
}

void exit_mm(Thread *thread)
{
    PDE *phy_pdir = (PDE *)va_to_pa(pdir[thread->pid]);

    uint32_t pdir_idx, ptable_idx;
    for (pdir_idx = 0; pdir_idx < KOFFSET / PD_SIZE; ++pdir_idx) {
        PDE *pde = &phy_pdir[pdir_idx];
        if (pde->present) {
            PTE *ptable = (PTE *)(pde->page_frame << 12);

            for (ptable_idx = 0; ptable_idx < NR_PTE; ++ptable_idx) {
                PTE *pte = &ptable[ptable_idx];

                if (pte->present) {
                    free_page(pte->page_frame << 12);

                    make_invalid_pte(pte);
                }
            }

            uint32_t index = ((void *)ptable - va_to_pa(ptables)) / PAGE_SIZE;
            ptmap[index / 8] &= ~(1 << index % 8);

            make_invalid_pde(pde);
        }
    }
}

void do_mmap(Thread *thread, uint32_t vaddr, uint32_t len)
{
    uint32_t taddr = vaddr >> 12 << 12;
    while (taddr < vaddr + len) {
        uint32_t pdir_idx, ptable_idx;

        pdir_idx = vaddr >> 22;
        ptable_idx = (vaddr >> 12) & 0x3ff;

        PDE *phy_pdir = (PDE *)va_to_pa(&pdir[thread->pid]);
        PDE *pde = &phy_pdir[pdir_idx];

        PTE *ptable = NULL;
        if (!pde->present) {
            ptable = (PTE*)va_to_pa(get_free_ptable());
            make_pde(pde, ptable);
        }

        ptable = (PTE *)(pde->page_frame << 12);
        PTE *pte = &ptable[ptable_idx];

        if (!pte->present) {
            uint32_t paddr = get_free_page();
            make_pte(pte, (void *)paddr);
        }

        taddr += PAGE_SIZE;
    }
}

uint32_t translate_vaddr(Thread *thread, uint32_t vaddr)
{
    if (!thread->mm_struct) {
        // kernel thread
        return vaddr;
    }

    uint32_t pdir_idx, ptable_idx;
    pdir_idx = vaddr >> 22;
    ptable_idx = (vaddr >> 12) & 0x3ff;

    PDE *phy_pdir = (PDE *)va_to_pa(&pdir[thread->pid]);
    PDE *pde = &phy_pdir[pdir_idx];

    assert(pde->present);

    PTE *ptable = (PTE *)(pde->page_frame << 12);
    PTE *pte = &ptable[ptable_idx];

    assert(pte->present);

    return pte->page_frame << 12 | (vaddr & 0xfff);
}
