#ifndef __X86_IO_H__
#define __X86_IO_H__

/* 读I/O端口 */
static inline uint8_t
in_byte(uint16_t port) {
    uint8_t data;
    asm volatile("in %1, %0" : "=a"(data) : "d"(port));
    return data;
}

static inline uint32_t
in_long(uint16_t port) {
    uint32_t data;
    asm volatile("in %1, %0" : "=a"(data) : "d"(port));
    return data;
}

/* 写I/O端口 */
static inline void
out_byte(uint16_t port, int8_t data) {
    asm volatile("out %%al, %%dx" : : "a"(data), "d"(port));
}

static inline void
out_long(uint16_t port, uint32_t data) {
    asm volatile("out %%eax, %%dx" : : "a"(data), "d"(port));
}

#endif
