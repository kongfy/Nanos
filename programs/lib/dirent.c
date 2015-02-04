#include "dirent.h"
#include "unistd.h"
#include "string.h"
#include "const.h"

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
