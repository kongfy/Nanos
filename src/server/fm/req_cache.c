#include "common.h"
#include "fsys.h"
#include "hal.h"
#include "server/fm.h"

#define MAX_REQ 256

static Request buffer[MAX_REQ];
static uint32_t map[MAX_REQ / 32];
static int msg_count = 0;

static inline
Request* get_free_buffer()
{
    int index = 0;
    msg_count += 1;
    for (index = 0; index < MAX_REQ; ++index) {
        uint32_t mask = 1U << (index % 32);
        if (!(map[index >> 5] & mask)) {
            map[index >> 5] |= mask;
            return &buffer[index];
        }
    }

    assert(0);
    return NULL;
}

void cache_request(Request_key key, Message *msg, PTR_CALLBACK callback)
{
    Request *request = get_free_buffer();
    request->key = key;
    request->msg = *msg;
    request->callback = callback;
}

static inline
bool compare_key(Request_key *a, Request_key *b)
{
    if (a->type != b->type) {
        return false;
    }

    if (a->type == REQ_FSYS) {
        return a->key.fsys.req_pid == b->key.fsys.req_pid;
    } else if (a->type == REQ_DEV) {
        return ((a->key.dev.pid == b->key.dev.pid) &&
                (a->key.dev.req_pid == b->key.dev.req_pid) &&
                (a->key.dev.dev_id == b->key.dev.dev_id));
    }

    return false;
}

void reply(Request_key key, int ret)
{
    int index = 0;
    for (index = 0; index < MAX_REQ; ++index) {
        uint32_t mask = 1U << (index % 32);
        if (map[index >> 5] & mask) {
            if (compare_key(&key, &(buffer[index].key))) {
                Message *m = &buffer[index].msg;
                FMMessage *msg = (FMMessage *)m;

                if (buffer[index].callback) {
                    ret = buffer[index].callback(msg, ret);
                }

                msg->ret = ret;
                send(m->src, m);
                map[index >> 5] &= ~mask;

                msg_count -= 1;
                return;
            }
        }
    }

    assert(0);
}
