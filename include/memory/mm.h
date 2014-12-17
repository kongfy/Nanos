#ifndef __MEMORY_MM_H__
#define __MEMORY_MM_H__

#include "kernel/kthread.h"

void allocate_mm(Thread *thread);
void exit_mm(Thread *Thread);
void copy_mm(Thread *parent, Thread *child);

void do_mmap(Thread *thread, uint32_t vaddr, uint32_t len);
uint32_t translate_vaddr(Thread *thread, uint32_t vaddr);

inline PDE* get_pdir(Thread *thread);
inline void set_tss_esp0(uint32_t esp);

#endif /* __MEMORY_MM_H__ */
