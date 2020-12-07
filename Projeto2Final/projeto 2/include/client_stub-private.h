#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H

#include "message-private.h"
#include "network_client.h"
#include "sdmessage.pb-c.h"
#include "serialization.h"

struct rtree_t {
    struct sockaddr_in *sock_in;
    int sockfd;
};

#endif