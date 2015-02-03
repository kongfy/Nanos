#ifndef KSH_H
#define KSH_H

#include "const.h"

static inline
bool isPathChar(char c)
{
    if ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') ||
        c == '/' || c == '.') {
        return true;
    }
    return false;
}

#endif /* KSH_H */
