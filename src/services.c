#include "napi/services.h"
#include "napi/list.h"

#include "internals/core.h"
#include "internals/api/api_protocol.h"
#include "internals/services/services.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct NP_Service_Callback
{
    uint64_t service_id;
    void(*events_callback)(NP_Service_Event);
};

extern struct NP_API_Conn *__api_conn;
static NP_List *events_pool;

void np_intr_services_client_init()
{
    events_pool = np_list_create();
}

void np_intr_services_client_destroy()
{
    np_list_destroy(events_pool);
}

void np_intr_services_accept_event(uint64_t service_id, NP_Service_Event event)
{
}

int32_t np_service_send_message(uint64_t service_id, NP_Service_Message message)
{
    // todo
}

int32_t np_service_unregister(uint64_t service_id)
{int32_t ret; struct NP_Service_Callback *nsh;
    if (__api_conn == NULL)
    {
        return NP_SERVICE_OFFLINE;
    }
    else if (!__api_conn->connected)
    {
        return NP_SERVICE_OFFLINE;
    }

    np_intr_server_handler_block();
    ret = np_intr_api_protocol_services_unregister_send(__api_conn, service_id);
    np_intr_server_handler_unblock();

    if (ret >> 8 == NP_API_RESPONSES_PREFIX)
    {
        if (np_intr_api_protocol_client_handle(__api_conn, ret) != NP_PROTOCOL_SUCCESS)
            return NP_SERVICE_INTERNAL_ERROR;
    }

    if (ret == NP_SERVICE_SUCCESS)
    {
        np_list_foreach(events_pool, item)
        {
            nsh = (struct NP_Service_Callback*)(*item)->value;
            if (nsh->service_id == service_id)
            {
                np_list_remove(events_pool, (*item)->id);
                free(nsh);
                break;
            }
        }
    }

    return ret;
}

int32_t np_service_register(uint64_t service_id, const char *name, uint8_t flags, void(*events_callback)(NP_Service_Event))
{int32_t ret; struct NP_Service_Callback *nsh;
    if (__api_conn == NULL)
    {
        return NP_SERVICE_OFFLINE;
    }
    else if (!__api_conn->connected)
    {
        return NP_SERVICE_OFFLINE;
    }

    if (strlen(name) > 256)
    {
        return NP_SERVICE_INCORRECT_NAME;
    }

    // block the server handler so we can receive the response
    np_intr_server_handler_block();
    ret = np_intr_api_protocol_services_register_send(__api_conn, service_id, name, flags);
    np_intr_server_handler_unblock();

    if (ret >> 8 == NP_API_RESPONSES_PREFIX)
    {
        if (np_intr_api_protocol_client_handle(__api_conn, ret) != NP_PROTOCOL_SUCCESS)
            return NP_SERVICE_INTERNAL_ERROR;
    }

    if (ret == NP_SERVICE_SUCCESS)
    {
        nsh = malloc(sizeof(struct NP_Service_Callback));
        nsh->service_id = service_id;
        nsh->events_callback = events_callback;
        np_list_append(events_pool, np_list_item(nsh));
    }

    return ret;
}

