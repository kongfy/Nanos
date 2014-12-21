#ifndef __TTY_H__
#define __TTY_H__

#define NR_TTY         4
#define MSG_TTY_GETKEY 1
#define MSG_TTY_UPDATE 2
#define LBUF_SZ        256
#define CBUF_SZ        1024
#define RSTK_SZ        16

#include "common.h"
#include "kernel.h"
#include "hal.h"

struct Console {
    const char *name;
    uint16_t *vbuf;
    uint16_t *scr;
    int w, h, wh;
    int size, pos;
    char lbuf[LBUF_SZ + 1];
    char cbuf[CBUF_SZ + 1];
    int i, f, r;
    int rtop;
    Message rstk[RSTK_SZ];
};
typedef struct Console Console;

extern pid_t TTY;
extern Console ttys[];
extern const char *ttynames[];
extern Console *current_consl;

void consl_sync(Console *c);
void consl_writec(Console *c, char ch);
void consl_feed(Console *c, int key);
void consl_accept(Console *c, char ch);
void readkey(void);
void update_banner(void);
void read_request(DevMessage *m);

#endif
