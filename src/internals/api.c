#include "api.h"
#include "internals/core.h"
#include "napi/core.h"
#include "napi/list.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

struct NP_API *np_intr_api_create()
{int32_t ret; struct NP_API *api;
    api = malloc(sizeof(struct NP_API));
    api->__id = 1;

    np_log(NP_DEBUG, "api_create: creating socket");

    api->sockaddr = malloc(sizeof(struct sockaddr_un));
    api->fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (api->fd == -1)
    {
        np_log(NP_ERROR, "api_create: unable to create UNIX Socket: %s", strerror(errno));
        goto err;
    }

    np_log(NP_DEBUG, "api_create: binding socket");

    api->sockaddr->sun_family = AF_UNIX;   
    strcpy(api->sockaddr->sun_path, NP_SOCKET_PATH); 
    
    unlink(NP_SOCKET_PATH);
    ret = bind(api->fd, (struct sockaddr *) api->sockaddr, sizeof(struct sockaddr_un));
    if (ret == -1)
    {
        np_log(NP_ERROR, "api_create: failed to bind the server: %s", strerror(errno));
        goto err;
    }

    // create a list for connections
    api->connections_pool = np_list_create();

    goto end;
err:
    np_intr_api_free(api);
    api = NULL;
end:
    return api;
}

struct NP_API_Conn *np_intr_api_connect(const char *client_path)
{int32_t ret; struct NP_API_Conn *conn;
    conn = malloc(sizeof(struct NP_API_Conn));
    conn->__id = 2;
    conn->side = 0; // client side
    conn->connected = false;

    np_log(NP_DEBUG, "api_connect: creating socket");

    conn->client_sockaddr = malloc(sizeof(struct sockaddr_un));
    conn->server_sockaddr = malloc(sizeof(struct sockaddr_un));
    conn->fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (conn->fd == -1)
    {
        np_log(NP_ERROR, "api_connect: failed to create UNIX Socket: %s", strerror(errno));
        goto err;
    }

    np_log(NP_DEBUG, "api_connect: binding socket");

    conn->client_sockaddr->sun_family = AF_UNIX;   
    strcpy(conn->client_sockaddr->sun_path, client_path); 
    
    unlink(client_path);
    ret = bind(conn->fd, (struct sockaddr *) conn->client_sockaddr, sizeof(struct sockaddr_un));
    if (ret == -1)
    {
        np_log(NP_ERROR, "api_connect: failed to bind: %s", strerror(errno));
        goto err;
    }

    np_log(NP_DEBUG, "api_connect: connecting to the socket server");

    conn->server_sockaddr->sun_family = AF_UNIX;   
    strcpy(conn->server_sockaddr->sun_path, NP_SOCKET_PATH); 
    ret = connect(conn->fd, (struct sockaddr *) conn->server_sockaddr, sizeof(struct sockaddr_un));
    if (ret == -1)
    {
        np_log(NP_ERROR, "api_connect: failed to connect to the server: %s", strerror(errno));
        goto err;
    }

    memcpy((void*)conn->version, NP_VERSION, 16);
    conn->version_id = NP_VERSION_ID;

    goto end;
err:
    np_intr_api_free(conn);
    conn = NULL;
end:
    return conn;
}

int64_t np_intr_api_poll_connection(struct NP_API *api)
{int32_t ret; struct NP_API_Conn *conn;
    do
    {
        ret = listen(api->fd, 1);
        if (ret == -1)
        {
            np_log(NP_PANIC, "api_poll_connection: unable to listen to connections: %s", strerror(errno));
        }
        
        conn = malloc(sizeof(struct NP_API_Conn));
        conn->__id = 2;
        conn->side = 1; // server side
        conn->connected = false;

        conn->client_sockaddr = malloc(sizeof(struct sockaddr_un));
        conn->fd = accept(api->fd, (struct sockaddr *) conn->client_sockaddr, &conn->len);
        if (conn->fd == -1)
        {
            np_log(NP_ERROR, "api_poll_connection: unable to accept connection: %s", strerror(errno));
            close(conn->fd);
            free(conn);
            continue;
        }
    } while (conn == NULL);

    conn->pool_id = np_list_append(api->connections_pool, np_list_item(conn));
    return conn->pool_id;
}

void np_intr_api_free(void *conn)
{
    if (conn == NULL)
        return;

    uint8_t id = *(uint8_t*)conn;
    
    if (id == 1)
    {   // NP_API
        np_log(NP_DEBUG, "api_free: freeing NP_API");
        struct NP_API *api = (struct NP_API*)conn;
        
        np_list_foreach(api->connections_pool, conn)
        {
            close(((struct NP_API_Conn*)(*conn)->value)->fd);
            free(((struct NP_API_Conn*)(*conn)->value)->client_sockaddr);
            free(((struct NP_API_Conn*)(*conn)->value)->server_sockaddr);
            free(((struct NP_API_Conn*)(*conn)->value));
        }

        close(api->fd);
        np_list_destroy(api->connections_pool);
        free(api->sockaddr);
        free(api);

        return;
    }
    else if (id == 2)
    {   // NP_API_conn   
        np_log(NP_DEBUG, "api_free: freeing NP_API_conn");
        struct NP_API_Conn *api = (struct NP_API_Conn*)conn;
        
        close(api->fd);
        free(api->client_sockaddr);
        free(api->server_sockaddr);
        free(api);

        return;
    }
    
    np_log(NP_ERROR, "Invalid API structure!");
}

