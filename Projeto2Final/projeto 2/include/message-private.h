#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "sdmessage.pb-c.h"

#define MAX_MSG 2048

// Estrutura auxiliar para guardas a estrutura criada pelo protobuffer
struct message_t {
    MessageT *msg;
};

int read_all(int socket, void *buf, int size);

int write_all(int socket, void* buf, int size);

#endif