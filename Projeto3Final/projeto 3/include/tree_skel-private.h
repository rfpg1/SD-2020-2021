#ifndef _TREE_SKEL__PRIVATE_H
#define _TREE_SKEL__PRIVATE_H

#include "sdmessage.pb-c.h"
#include <signal.h>

struct task_t {
    int op_n;
    int op;
    char *key;
    char *data;
    int datasize;
    struct task_t *nextTask;
};

int init_mutex_cond();

void mutex_cond_destroy();

struct task_t *getTask();

void task_destroy(struct task_t *tarefa);

void add_task(struct task_t *tarefa);

struct task_t* task_create(int op_n, int op, char* key, char* data, int dataSize);
#endif