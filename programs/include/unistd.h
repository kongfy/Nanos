#ifndef __UNISTD_H__
#define __UNISTD_H__

#include "types.h"

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

// PM
pid_t getpid(void);
pid_t fork(void);
int exec(const char *filename, char *const argv[]);
int waitpid(pid_t pid);
unsigned int sleep(unsigned int seconds);
void exit(int status);

// FM
int open(const char *filename);
int lseek(int fd, int offset, int whence);
int close(int fd);
int dup(int oldfd);
int dup2(int oldfd, int newfd);
int pipe(int pipefd[2]);
int read(int fd, uint8_t *buf, int len);
int write(int fd, uint8_t *buf, int len);

int chdir(const char *path);
int lsdir(const char *path, uint8_t *buf);
int mkdir(const char *path);
int rmdir(const char *path);
int unlink(const char *path);

#endif /* __UNISTD_H__ */
