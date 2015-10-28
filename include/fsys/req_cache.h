#ifndef __FSYS_REQ_CACHE_H__
#define __FSYS_REQ_CACHE_H__

#include "server/fm.h"

typedef int (*PTR_CALLBACK)(FMMessage *msg, int ret);

typedef enum Request_type {
    REQ_NULL = 0,
    REQ_FSYS = 1,
    REQ_DEV = 2,
} Request_type;

typedef union Request_key_data {
    struct
    {
        pid_t req_pid;
    } fsys;

    struct
    {
        pid_t pid;
        int dev_id;
        pid_t req_pid;
    } dev;
} Request_key_data;

typedef struct Request_key
{
    Request_type type;
    Request_key_data key;
} Request_key;

typedef struct Request
{
    Request_key key;
    // value
    Message msg;
    PTR_CALLBACK callback;
} Request;

void cache_request(Request_key key, Message *msg, PTR_CALLBACK callback);
void reply(Request_key key, int ret);

#endif /* __FSYS_REQ_CACHE_H__ */
