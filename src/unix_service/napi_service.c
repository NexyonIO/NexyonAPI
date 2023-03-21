#include "internals/core.h"
#include "napi/core.h"
#include "napi/list.h"

#include "unix_service/connection_handler.h"
#include "internals/services/services.h"
#include "internals/api/api.h"

#include <execinfo.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

struct NP_API *api;
bool running;

void signal_handler(int type);

int main(void)
{
    np_log_prefix("napid");

    np_log(NP_INFO, "running on:");
    np_log(NP_INFO, "\tkernel: %s", np_intr_platform_info(NP_INTR_KERNEL_INFO));
    np_log(NP_INFO, "\thardware: %s", np_intr_platform_info(NP_INTR_HARDWARE_INFO));

    np_log(NP_INFO, "Installing signal handlers");
    signal(SIGKILL, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGSEGV, signal_handler);

    np_log(NP_INFO, "main: creating socket server");
    api = np_intr_api_create();

    np_log(NP_INFO, "main: initializing services");
    np_intr_services_server_init();
    
    running = true;
    while (running)
    {
        int64_t conn_id = np_intr_api_poll_connection(api);
        struct NP_API_Conn *conn = np_list_get_item(api->connections_pool, conn_id)->value;
        np_log(NP_DEBUG, "main: new client %d", conn_id);

        pthread_create(
            &conn->conn_thread,
            NULL,
            np_service_connection_handler,
            conn
        );
    }

    np_intr_services_server_destroy();
    np_intr_api_free(api);
    return 0;
}

void signal_handler(int type)
{void *array[32]; char **strings; int size; size_t i;
    if (type == SIGINT)
    {
        np_log(NP_DEBUG, "SIGINT: might be ctrl + c");
    }
    else if (type == SIGKILL)
    {
        np_log(NP_DEBUG, "SIGKILL: killing process");
    }
    else if (type == SIGTERM)
    {
        np_log(NP_DEBUG, "SIGTERM: terminating process");
    }
    else if (type == SIGSEGV)
    {
        np_log(NP_ERROR, "SIGSEGV: segmentation fault!");
        
        // print stack trace for debugging
        size = backtrace(array, 32);
        strings = backtrace_symbols(array, size);
        if (strings != NULL)
        {
            np_log(NP_DEBUG, "SIGSEGV: obtained %d stack frames", size);
            for (i = 0; i < size; ++i)
            {
                np_log(NP_DEBUG, "SIGSEGV:\t%s", strings[i]);
            }
        }

        free(strings);
        abort();
    }

    np_intr_api_free(api);
    api = NULL;

    running = false;
}

