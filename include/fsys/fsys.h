#ifndef __FSYS_FSYS_H__
#define __FSYS_FSYS_H__

#include "kernel/kthread.h"

size_t ram_read(int file_name, uint8_t *buf, off_t offset, size_t len, Thread *thread);

void init_fm_tty(Thread *thread, int tty);
void copy_fm(Thread *parent, Thread *child);
void exit_fm(Thread *Thread);

int do_open(Thread *thread, int filename);
int do_close(Thread *thread, int fd);
int do_read(Thread *thread, int fd, uint8_t *buf, int len);
int do_write(Thread *thread, int fd, uint8_t *buf, int len);
int do_lseek(Thread *Thread, int fd, int offset, int whence);
int do_dup(Thread *thread, int oldfd);
int do_dup2(Thread *thread, int oldfd, int newfd);
int do_pipe(Thread *Thread, int pipefd[2]);

#endif /* __FSYS_FSYS_H__ */