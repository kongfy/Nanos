#include "common.h"

/* 注意！itoa只有一个缓冲，因此
 * char *p = itoa(100);
 * char *q = itoa(200);
 * 后p和q所指内容都是"200"。
 */
char *itoa(int a) {
	static char buf[30];
	char *p = buf + sizeof(buf);
	do {
		*--p = '0' + a % 10;
	} while (a /= 10);
	return p;
}

void *
memcpy(void *dst, const void *src, unsigned int count) {
    void *ret = dst;
    while (count --) {
        *(char *)dst = *(char *)src;
        dst = (char *)dst + 1;
        src = (char *)src + 1;
    }
    return ret;
}

void *
memset(void *dst, int val, unsigned int count) {
    void *start = dst;
    while (count --) {
        *(char *)dst = (char)val;
        dst = (char *)dst + 1;
    }
    return start;
}

