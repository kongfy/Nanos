#ifndef __DIRENT_H__
#define __DIRENT_H__

#include "types.h"

#define MAX_ENTRY 512
#define MAX_PATH_LEN 2048

typedef struct dirent {
    char filename[32];
    int inode;
} dirent;

typedef struct DIR {
    int p, total;
    dirent entrys[MAX_ENTRY];
} DIR;

int opendir(const char *pathname, DIR *dp);
struct dirent *readdir(DIR *dp);
char *getcwd(char *buf, size_t size);

#endif /* __DIRENT_H__ */
