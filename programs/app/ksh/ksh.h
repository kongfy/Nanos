#ifndef KSH_H
#define KSH_H

#include "const.h"

static inline
bool is_path_char(const char c)
{
    if ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') ||
        c == '/' || c == '.' || c == '_') {
        return true;
    }
    return false;
}

static inline
bool is_blank_char(const char c)
{
    return (' ' == c || '\t' == c || '\n' == c);
}

#endif /* KSH_H */
