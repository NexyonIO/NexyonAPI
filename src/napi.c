#include "napi/core.h"
#include "napi/list.h"

#include "internals/api_protocol.h"
#include "internals/api.h"
#include "internals/core.h"

#include <unistd.h>
#include <sys/socket.h>

struct NP_API_Conn *__api_conn;

extern int32_t np_main(int32_t argc, char **argv)
{int32_t ret;
    np_log_prefix("napi");

    np_log(NP_INFO, "running on:");
    np_log(NP_INFO, "\tkernel: %s", np_intr_platform_info(NP_INTR_KERNEL_INFO));
    np_log(NP_INFO, "\thardware: %s", np_intr_platform_info(NP_INTR_HARDWARE_INFO));

    __api_conn = np_intr_api_connect("napid_unix_socket.client");

    np_log(NP_DEBUG, "np_main: attempting to handshake");
    ret = np_intr_api_protocol_handshake_send(__api_conn);

    if (ret != NP_PROTOCOL_SUCCESS)
    {
        np_log(NP_ERROR, "np_main: failed to handshake (%s)", PROTOCOL_RESULT_STRINGS[ret]);
        goto err;
    }

    char buff[1];
    while (__api_conn->connected)
    {
        
        
        usleep(50 * 1000);
    }

    goto end;
err:
    np_intr_api_free(__api_conn);
    return 1;
end:
    return 0;
}

