#include "common.h"
#include "memory.h"
#include "string.h"

#define TOTAL_USER_PAGES ((PHY_MEM - KMEM) / PAGE_SIZE)

static page pages[TOTAL_USER_PAGES];

static
void clear_page(uint32_t index)
{
    memset((void *)(KMEM + index * PAGE_SIZE), 0, PAGE_SIZE);
}

uint32_t get_free_page()
{
    int index = 0;
    for (index = 0; index < TOTAL_USER_PAGES; ++index) {
        if (!pages[index]._count) {
            clear_page(index);
            pages[index]._count ++;
            return KMEM + index * PAGE_SIZE;
        }
    }

    assert(0); // all pages used up.
    return 0;
}

void free_page(uint32_t addr)
{
    assert(addr > KMEM);
    assert(addr < PHY_MEM);

    int index = (addr - KMEM) / PAGE_SIZE;
    pages[index]._count --;
}
