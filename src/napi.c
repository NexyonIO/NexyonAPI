#include "napi/core.h"
#include "napi/list.h"
#include "napi/services.h"

#include "internals/result_codes.h"
#include "internals/services/services.h"
#include "internals/api/api_protocol.h"
#include "internals/api/api.h"
#include "internals/core.h"

#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>

pthread_t __np_thread;
struct NP_API_Conn *__api_conn;
bool initialized = false;

int32_t np_main(int32_t argc, char **argv)
{int32_t ret;
    if (!initialized)
    {
        np_log_prefix("napi");

        np_log(NP_INFO, "running on:");
        np_log(NP_INFO, "\tkernel: %s", np_intr_platform_info(NP_INTR_KERNEL_INFO));
        np_log(NP_INFO, "\thardware: %s", np_intr_platform_info(NP_INTR_HARDWARE_INFO));

        initialized = true;
    }
    else
    {
        np_log(NP_INFO, "np_main: reinitializing");
    }

    __api_conn = np_intr_api_connect("napid_unix_socket.client");

    if (__api_conn != NULL)
    {
        np_log(NP_DEBUG, "np_main: attempting to handshake");
        ret = np_intr_api_protocol_handshake_send(__api_conn);

        if (ret != NP_PROTOCOL_SUCCESS)
        {
            np_log(NP_ERROR, "np_main: failed to handshake (%s)", np_return_value_meaning(ret));
            goto err;
        }
    }
    else 
    {
        np_log(NP_ERROR, "np_main: failed connect to the napid");
        goto err;
    }

    np_log(NP_INFO, "np_main: initializing services");
    np_intr_services_client_init();

    // unblocking server thread
    np_intr_server_handler_unblock();

    goto end;
err:
    np_destroy();
    return 1;
end:
    return 0;
}

bool np_alive()
{
    if (__api_conn == NULL)
    {
        return false;
    } 

    return __api_conn->connected;
}

int32_t np_destroy()
{
    if (__api_conn == NULL)
    {
        return 1;
    }

    np_intr_services_client_destroy();
    np_intr_api_free(__api_conn);
    np_intr_server_handler_block();

    __api_conn = NULL;

    return 0;
}

const char *np_return_value_meaning(int32_t value)
{
    if (value == 0x00)
    {
        return "success";
    }

    return NP_RESULT_STRINGS[value];
}

void np_intr_server_handler_block()
{
    np_log(NP_INFO, "server_handler_block: blocking server handler thread");
    pthread_cancel(__np_thread);
}

void np_intr_server_handler_unblock()
{
    np_log(NP_INFO, "server_handler_unblock: unblocking server handler thread");
    pthread_create(&__np_thread, NULL, &np_intr_server_handler, NULL);
}

void np_intr_server_handler(void *unused)
{int32_t ret;
    do
    {
        ret = np_intr_api_protocol_execute(__api_conn);

        if (ret != NP_PROTOCOL_SUCCESS)
        {
            np_log(NP_WARN, "server_handler: failed to fetch packet (%s)", np_return_value_meaning(ret));
            np_destroy();
            return;
        }

        usleep(50 * 1000);
    }
    while (__api_conn->connected && ret == NP_PROTOCOL_SUCCESS);
}


