#ifndef __MEMORY_PAGE_H__
#define __MEMORY_PAGE_H__

#include "common.h"

uint32_t get_free_page();
void free_page(uint32_t addr);

#endif /* __MEMORY_PAGE_H__ */
