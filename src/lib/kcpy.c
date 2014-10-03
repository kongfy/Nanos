#include "kernel.h"
#include "string.h"
#include "memory.h"


void copy_from_kernel(Thread* pcb, void* dest, void* src, int len)
{
    int i = 0;

    for (i = 0; i < len; ++i) {
        char* pdest = (char*)translate_va(pcb, (uint32_t)(dest++));
        memcpy(pdest, src++, 1);
    }
}

void copy_to_kernel(Thread* pcb, void* dest, void* src, int len)
{
    int i = 0;

    for (i = 0; i < len; ++i) {
        char* psrc = (char*)translate_va(pcb, (uint32_t)(src++));
        memcpy(dest++, psrc, 1);
    }
}

void strcpy_to_kernel(Thread* pcb, char* dest, char* src)
{
    char *psrc;
    do {
        psrc = (char*)translate_va(pcb, (uint32_t)(src++));
        memcpy(dest++, psrc, 1);
    } while (*psrc != '\0');
}

void strcpy_from_kernel(Thread* pcb, char* dest, char* src)
{
    copy_from_kernel(pcb, dest, src, strlen(src));
}
