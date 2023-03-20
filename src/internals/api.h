#pragma once

#include "napi/list.h"

#include "core.h"

#include <pthread.h>
#include <sys/socket.h>

// 3 sec tiemout
#define NP_API_TIMEOUT 3000
#define NP_SOCKET_PATH "napi_unix_sock.server"

struct NP_API
{
    uint8_t __id;

    int32_t fd;
    struct sockaddr_un *sockaddr;

    NP_List *connections_pool;
};

struct NP_API_Conn
{
    uint8_t __id;
 
    bool connected;
    bool side;      // 1 -- server side, 0 -- client side (remote connection)
    int64_t pool_id;

    int32_t fd;
    struct sockaddr_un *client_sockaddr;
    struct sockaddr_un *server_sockaddr;

    const char *version[16];
    uint32_t version_id;

    socklen_t len;
    pthread_t conn_thread;
};

struct NP_API *np_intr_api_create();
struct NP_API_Conn *np_intr_api_connect(const char *client_path);
int64_t np_intr_api_poll_connection(struct NP_API *api);
void np_intr_api_free(void *conn);

