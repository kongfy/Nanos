#ifndef __COMMON_ASSERT_H__
#define __COMMON_ASSERT_H__

#include "stdio.h"

void abort(const char *, int);

/* assert: 断言条件为真，若为假则蓝屏退出 */
#define assert(cond) do { \
    if (!(cond)) { \
        printf("\nAssertion failed in function \"%s\": %s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
        abort(__FILE__, __LINE__); \
    } \
} while (0)

/* system panic */
#define panic(...) do { \
    printf("\n\33[1;31mSystem panic\33[0m in function \"%s\", line %d, file \"%s\":\n", \
        __FUNCTION__, __LINE__, __FILE__); \
    printf(__VA_ARGS__); \
    abort(__FILE__, __LINE__); \
} while(0)

#endif
