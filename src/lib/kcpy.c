#include "kernel.h"
#include "string.h"
#include "memory.h"

static inline
int min (int a, int b)
{
    return a < b ? a : b;
}

uint32_t copy_from_kernel(Thread* pcb, void* dest, void* src, int len)
{
    uint32_t count = 0;
    int l;

    while (len > 0) {
        char* pdest = (char*)translate_vaddr(pcb, (uint32_t)dest);
        l = min(((((uint32_t)pdest >> 12) + 1) << 12) - (uint32_t)pdest, len);
        memcpy(pdest, src, l);

        len -= l;
        dest += l;
        src += l;
        count += l;
    }

    return count;
}

uint32_t copy_to_kernel(Thread* pcb, void* dest, void* src, int len)
{
    uint32_t count = 0;
    int l;

    while (len > 0) {
        char* psrc = (char*)translate_vaddr(pcb, (uint32_t)src);
        l = min(((((uint32_t)psrc >> 12) + 1) << 12) - (uint32_t)psrc, len);
        memcpy(dest, psrc, l);

        len -= l;
        dest += l;
        src += l;
        count += l;
    }

    return count;
}

uint32_t strcpy_to_kernel(Thread* pcb, char* dest, char* src)
{
    char *psrc;
    int count = 0;
    do {
        psrc = (char*)translate_vaddr(pcb, (uint32_t)(src++));
        memcpy(dest++, psrc, 1);
        count++;
    } while (*psrc != '\0');

    return count;
}

uint32_t strcpy_from_kernel(Thread* pcb, char* dest, char* src)
{
    return copy_from_kernel(pcb, dest, src, strlen(src) + 1);
}
