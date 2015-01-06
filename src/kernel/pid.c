/*
 * pid.c
 *
 *  Created on: 2014-5-9
 *      Author: kongfy
 */

#include "kernel/pid.h"
#include "common.h"

#define MAP_LENGTH (MAX_PROCESS / sizeof(uint32_t) + 1)

static pid_t max_pid = 0;
static uint32_t pid_map[MAP_LENGTH];

static void
set_map(pid_t pid)
{
    int index = pid / 32;
    int reminder = pid % 32;

    pid_map[index] |= 1 << reminder;
}

static inline
void clear_map(pid_t pid)
{
    int index = pid / 32;
    int reminder = pid % 32;

    pid_map[index] &= ~(1 << reminder);
}

bool is_pid_available(pid_t pid)
{
    int index = pid / 32;
    int reminder = pid % 32;

    if (pid_map[index] & (1 << reminder)) {
        return FALSE;
    } else {
        return TRUE;
    }
}

static inline
pid_t next_available_pid(pid_t last)
{
    int pid;

    for (pid = last + 1; pid <= MAX_PID; ++pid) {
        if (is_pid_available(pid)) {
            return pid;
        }
    }

    for (pid = 0; pid <= last; ++pid) {
        if (is_pid_available(pid)) {
            return pid;
        }
    }

    return -1;
}

pid_t get_free_pid(void)
{
    static pid_t last = -1;

    pid_t pid = next_available_pid(last);
    if (pid >= 0) {
        last = pid;
        set_map(pid);
        return pid;
    }

    return -1;
}

void free_pid(pid_t pid)
{
    clear_map(pid);
}

void init_pid(void)
{
    uint32_t i;

    for (i = 0; i < MAP_LENGTH; ++i) {
        pid_map[i] = 0;
    }
    max_pid = 0;
}
