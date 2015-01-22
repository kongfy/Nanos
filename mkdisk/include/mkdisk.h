#ifndef __MKDISK_H__
#define __MKDISK_H__


typedef enum MKDError {
    ESTAT = -1,
    ENDIR = -2,
} MKDError;

MKDError mkdisk(const char *pathname, int fd);

#endif /* __MKDISK_H__ */
