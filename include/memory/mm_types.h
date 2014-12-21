#ifndef __MEMORY_MM_TYPES_H__
#define __MEMORY_MM_TYPES_H__

#include "x86/memory.h"

typedef struct mm_struct {
    PDE *pgd;
} mm_struct;

typedef struct page {
    uint8_t _count; // page reference count
} page;

#endif /* __MEMORY_MM_TYPES_H__ */
