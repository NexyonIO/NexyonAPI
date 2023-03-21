#include "napi/core.h"
#include "napi/list.h"

#include "internals/api/api_protocol.h"
#include "internals/api/api.h"
#include "unix_service/connection_handler.h"

#include <unistd.h>
#include <pthread.h>

extern struct NP_API *api;

void np_service_connection_handler(void *ptr)
{struct NP_API_Conn *conn = ptr; int32_t ret;
    np_log(NP_DEBUG, "connection_handler[%d]: thread started.", conn->pool_id);

    do
    {
        ret = np_intr_api_protocol_execute(conn);

        if (ret != NP_PROTOCOL_SUCCESS)
        {
            np_log(NP_WARN, "connection_handler[%d]: failed to fetch packet (%s)", conn->pool_id, np_return_value_meaning(ret));
            goto destroy;   
        }

        usleep(50 * 1000);
    }
    while (conn->connected && ret == NP_PROTOCOL_SUCCESS);
    
destroy:
    // cleanup everything and destroy thread
    np_log(NP_DEBUG, "connection_handler[%d]: cleaning up thread. Current client amount: %ld",
            conn->pool_id, np_list_length(api->connections_pool)-1);
    np_intr_api_free(conn);
    np_list_remove(api->connections_pool, conn->pool_id);
    pthread_cancel(conn->conn_thread);
}

