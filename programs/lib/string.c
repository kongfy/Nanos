#include "string.h"

/* 注意！itoa只有一个缓冲，因此
 * char *p = itoa(100);
 * char *q = itoa(200);
 * 后p和q所指内容都是"200"。
 */
char *itoa(int a) {
    static char buf[30];
    char *p = buf + sizeof(buf) - 1;
    do {
        *--p = '0' + a % 10;
    } while (a /= 10);
    return p;
}

void memcpy(void *dest, const void *src, size_t size) {
    asm volatile ("cld; rep movsb" : : "c"(size), "S"(src), "D"(dest));
}

void memset(void *dest, uint8_t data, size_t size) {
    asm volatile ("cld; rep stosb" : : "c"(size), "a"(data), "D"(dest));
}

size_t strlen(const char *str) {
    int len = 0;
    while (*str ++) len ++;
    return len;
}

void strcpy(char *d, const char *s) {
    memcpy(d, s, strlen(s) + 1);
}

int strcmp(const char *s1, const char *s2) {
    unsigned char c1, c2;

    while (1) {
        c1 = *s1++;
        c2 = *s2++;
        if (c1 != c2)
            return c1 < c2 ? -1 : 1;
        if (!c1)
            break;
    }
    return 0;
}
