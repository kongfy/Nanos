#ifndef CHARSET_H
#define CHARSET_H

#include "common.h"
#include "string.h"

static inline
bool is_blank_char(const char c)
{
    return (' ' == c || '\t' == c || '\n' == c);
}

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
bool check_path_charset(const char *path)
{
    int len = strlen(path);
    int i = 0;

    for (i = 0; i < len; ++i) {
        if (!is_path_char(path[i])) {
            return false;
        }
    }

    return true;
}

#endif /* CHARSET_H */
