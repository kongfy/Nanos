#include "kernel.h"
#include "string.h"
#include "memory.h"


uint32_t copy_from_kernel(Thread* pcb, void* dest, void* src, int len)
{
    int i = 0;
    uint32_t count = 0;
    for (i = 0; i < len; ++i) {
        char* pdest = (char*)translate_va(pcb, (uint32_t)(dest++));
        memcpy(pdest, src++, 1);
        count++;
    }
    return count;
}

uint32_t copy_to_kernel(Thread* pcb, void* dest, void* src, int len)
{
    int i = 0;
    uint32_t count = 0;
    for (i = 0; i < len; ++i) {
        char* psrc = (char*)translate_va(pcb, (uint32_t)(src++));
        memcpy(dest++, psrc, 1);
        count++;
    }
    return count;
}

uint32_t strcpy_to_kernel(Thread* pcb, char* dest, char* src)
{
    char *psrc;
    int count = 0;
    do {
        psrc = (char*)translate_va(pcb, (uint32_t)(src++));
        memcpy(dest++, psrc, 1);
        count++;
    } while (*psrc != '\0');

    return count;
}

uint32_t strcpy_from_kernel(Thread* pcb, char* dest, char* src)
{
    return copy_from_kernel(pcb, dest, src, strlen(src) + 1);
}
