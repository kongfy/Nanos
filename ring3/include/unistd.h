#ifndef __UNISTD_H__
#define __UNISTD_H__

#include "types.h"

pid_t getpid(void);
pid_t fork(void);

int exec(int filename, char *const argv[]);

#endif /* __UNISTD_H__ */
