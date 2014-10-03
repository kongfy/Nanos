/*
 * vm.c

 *
 *  Created on: 2014-9-17
 *      Author: kongfy
 */


#include "kernel.h"
#include "kernel/pid.h"

#include "memory.h"

// 创建一个存放页表的空间
#define TOTAL_MAX_PTE (32 * (PHY_MEM / PAGE_SIZE))

// 用户进程虚拟内存
static CR3 cr3s[MAX_PROCESS];                                            // 用户进程 CR3
static PDE pdir[MAX_PROCESS][NR_PDE] align_to_page;                      // 用户进程 page directory
static PTE ptable_pool[TOTAL_MAX_PTE] align_to_page;                     // 用户进程 page tables

// static char pfmap[(PHY_MEM - KMEM) / PAGE_SIZE / 8];

static unsigned int base = KMEM;

static inline
unsigned int max(unsigned int a, unsigned int b)
{
    if (a > b) return a;
    return b;
}

static inline
unsigned int min(unsigned int a, unsigned int b)
{
    if (a < b) return a;
    return b;
}

inline CR3* get_cr3(Thread *thread)
{
    if (cr3s[thread->pid].val) {
        return &cr3s[thread->pid];
    } else {
        return get_kcr3();
    }
}

inline PDE* get_pdir(Thread *thread)
{
    return pdir[thread->pid];
}

void create_vm(Thread *thread)
{
    PDE *ppdir = (PDE *)va_to_pa(pdir[thread->pid]);

    uint32_t pdir_idx;

    for (pdir_idx = 0; pdir_idx < NR_PDE; pdir_idx ++) {
        make_invalid_pde(&ppdir[pdir_idx]);
    }

    // 高位内存映射到内核空间
    PDE *kpdir = (PDE *)va_to_pa(get_kpdir());
    for (pdir_idx = 0; pdir_idx < KMEM / PD_SIZE; pdir_idx ++) {
        PDE *kpde = &kpdir[pdir_idx + KOFFSET / PD_SIZE];
        uint32_t pframe_idx = kpde->page_frame << 12;
        make_pde(&ppdir[pdir_idx + KOFFSET / PD_SIZE], (void*)pframe_idx);
    }

    /* make CR3 to be the entry of page directory */
    CR3* cr3 = &cr3s[thread->pid]; // 注意此处不能使用get_cr3函数，会引起kcr3被修改
    cr3->val = 0;
    cr3->page_directory_base = ((uint32_t)ppdir) >> 12;
}

// 得到一页页表的第一个页表项，注意页表项是连续使用1024个的
static inline
PTE* get_free_ptable()
{
    PTE *pte = (PTE *)&ptable_pool;

    while ((uint32_t)pte <= (uint32_t)&ptable_pool[TOTAL_MAX_PTE]) {
        if (!pte->val) {
            return pte;
        }

        pte = (PTE *)((void *)pte + PAGE_SIZE); // 前进一页
    }

    assert(0); // 内存用光了
    return NULL;
}

// 简单水位线实现
uint32_t alloc_pages(Thread *thread, uint32_t vaddr, uint32_t memsz)
{
    uint32_t pa_start = base;
    uint32_t pa_end = (((base + memsz) >> 12) + ((memsz & 0xfff) > 0)) << 12;
    base = pa_end;

    uint32_t va_start = vaddr;
    uint32_t va_end = (((va_start + memsz) >> 12) + ((memsz & 0xfff) > 0)) << 12;

    // 根据分配的pa_start填写页表
    uint32_t pdir_idx, ptable_idx, pframe_idx;
    pframe_idx = pa_start >> 12;
    PDE *ppdir = (PDE *)va_to_pa(&pdir[thread->pid]);
    uint32_t addr = va_start;
    do {
        pdir_idx = addr >> 22;
        ptable_idx = (addr >> 12) & 0x3ff;

        PDE *pde = &ppdir[pdir_idx];
        if (!pde->val) {
            PTE *ptes = (PTE*)va_to_pa(get_free_ptable());
            make_pde(pde, ptes);
        }

        PTE *ptable = (PTE *)(pde->page_frame << 12);
        PTE *pte = &ptable[ptable_idx];

        assert(!pte->val); // 这个页表一定是没有分配的
        make_pte(pte, (void *)(pframe_idx << 12));
        pframe_idx++;
        addr += PAGE_SIZE;
    } while (addr < va_end);

    return pa_start | (vaddr & 0xfff);
}

uint32_t translate_va(Thread *thread, uint32_t vaddr)
{
    if (!cr3s[thread->pid].val) {
        // kernel thread
        return vaddr;
    }

    uint32_t pdir_idx, ptable_idx;
    pdir_idx = vaddr >> 22;
    ptable_idx = (vaddr >> 12) & 0x3ff;

    PDE *ppdir = (PDE *)va_to_pa(&pdir[thread->pid]);
    PDE *pde = &ppdir[pdir_idx];

    assert(pde->val);

    PTE *ptable = (PTE *)(pde->page_frame << 12);
    PTE *pte = &ptable[ptable_idx];

    assert(pte->val);

    return pte->page_frame << 12 | (vaddr & 0xfff);
}
