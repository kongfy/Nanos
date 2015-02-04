#ifndef __DIRENT_H__
#define __DIRENT_H__

#define MAX_ENTRY 512

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

#endif /* __DIRENT_H__ */
