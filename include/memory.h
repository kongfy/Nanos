#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "x86.h"
#include "kernel/kthread.h"

#define KOFFSET 0xC0000000

inline CR3* get_kcr3();
inline PDE* get_kpdir();
inline PTE* get_kptable();

void init_page(void);
void init_segment(void);

void make_invalid_pde(PDE *);
void make_invalid_pte(PTE *);
void make_pde(PDE *, void *);
void make_pte(PTE *, void *);

inline CR3* get_cr3(Thread *thread);
inline PDE* get_pdir(Thread *thread);

void create_vm(Thread *thread);
unsigned int alloc_pages(Thread *thread, unsigned int vaddr, unsigned int memsz);

#define va_to_pa(addr) \
    ((void*)(((uint32_t)(addr)) - KOFFSET))

#define pa_to_va(addr) \
    ((void*)(((uint32_t)(addr)) + KOFFSET))

/* the maxinum kernel size is 16MB */
#define KMEM    (16 * 1024 * 1024)

/* Nanos has 128MB physical memory  */
#define PHY_MEM   (128 * 1024 * 1024)

#endif
