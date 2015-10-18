#include "dirent.h"
#include "unistd.h"
#include "string.h"
#include "const.h"
#include "stdio.h"

int opendir(const char *pathname, DIR *dp)
{
    memset(dp, 0, sizeof(DIR));
    dp->total = lsdir(pathname, (uint8_t *)dp->entrys);
    return dp->total;
}

struct dirent *readdir(DIR *dp)
{
    if (!dp) return NULL;

    if (dp->p < dp->total) {
        return &dp->entrys[dp->p++];
    }

    return NULL;
}

static DIR dir;
static char pathbuf[MAX_PATH_LEN];
static inline
int inode_in_dir(char *filename, DIR *dp)
{
    int i = 0;
    for (i = 0; i < dp->total; ++i) {
        if (strcmp(filename, dp->entrys[i].filename) == 0) {
            return dp->entrys[i].inode;
        }
    }
    return -1;
}

static inline
char* filename_in_dir(int inode, DIR *dp)
{
    int i = 0;
    for (i = 0; i < dp->total; ++i) {
        if (inode == dp->entrys[i].inode) {
            return dp->entrys[i].filename;
        }
    }
    return NULL;
}

static inline
void reverse_word(char *buf, int l, int r)
{
    while (l < r) {
        char t = buf[l];
        buf[l] = buf[r];
        buf[r] = t;
        l++;
        r--;
    }
}

static inline
void reverse_path(char *path)
{
    int len = strlen(path);

    int l = 0;
    int r = len - 1;
    reverse_word(path, l, r);

    l = 0;
    r = l;
    while (l < len) {
        while (path[r] != '/' && r < len) r++;
        reverse_word(path, l, r - 1);
        l = r + 1;
        r = l;
    }
}

char *getcwd(char *buf, size_t size)
{
    if (!buf) return NULL;

    int error = opendir(".", &dir);
    if (error < 0) return NULL;

    char *p = buf;
    int target = inode_in_dir(".", &dir);
    int dot = inode_in_dir(".", &dir);
    int dotdot = inode_in_dir("..", &dir);
    pathbuf[0] = '\0';

    while (true) {
        if (dot == dotdot) {
            // root
            if (p == buf) {
                *(p++) = '/';
            }
            *p = '\0';
            break;
        }

        int t = strlen(pathbuf);
        strcpy(&pathbuf[t], "../");
        error = opendir(pathbuf, &dir);
        if (error < 0) return NULL;

        target = dot;
        dot = inode_in_dir(".", &dir);
        dotdot = inode_in_dir("..", &dir);

        char *filename = filename_in_dir(target, &dir);
        int len = strlen(filename);
        strcpy(p, filename);
        p += len;
        strcpy(p, "/");
        p += 1;
    }

    reverse_path(buf);

    return buf;
}
