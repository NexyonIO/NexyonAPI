#include "napi/core.h"
#include "napi/list.h"
#include "napi/services.h"

#include "lib/internals/api/api.h"
#include "lib/internals/api/api_protocol.h"
#include "lib/internals/services/services.h"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct NP_Service
{
    uint64_t service_id;
    const char name[256];
    uint8_t flags;
    struct NP_API_Conn *conn_ptr;
};

NP_List *services;
static bool services_busy = false;

void np_intr_services_server_init()
{
    services = np_list_create();
}

void np_intr_services_server_destroy()
{
    np_list_destroy(services);
}

int32_t np_intr_services_register(struct NP_API_Conn *conn, uint64_t service_id, const char *name, uint8_t flags)
{struct NP_Service *srv;
    // wait until the services manager would not be busy
    while (services_busy);
    services_busy = true;

    // check that service with the same id does not exist
    np_list_foreach(services, item)
    {
        srv = (struct NP_Service*)(*item)->value;
        if (srv->service_id == service_id)
        {
            services_busy = false;
            return NP_SERVICE_ALREADY_EXISTS;
        }
    }

    srv = malloc(sizeof(struct NP_Service));
    srv->service_id = service_id;
    srv->flags = flags;
    srv->conn_ptr = conn;
    memcpy((char*)srv->name, name, 256);
    np_list_append(services, np_list_item(srv));

    services_busy = false;
    np_log(NP_DEBUG, "services_register: service '%s'[%ld] has been registered. Services amount: %ld", name, service_id, services->__items_size);

    return NP_SERVICE_SUCCESS;
}

int32_t np_intr_services_unregister(uint64_t service_id)
{struct NP_Service *srv; char tmp_name[256];
    // wait until the services manager would not be busy
    while (services_busy);
    services_busy = true;

    // check that service with the same id exists
    np_list_foreach(services, item)
    {
        srv = (struct NP_Service*)(*item)->value;
        if (srv->service_id == service_id)
        {
            np_list_remove(services, (*item)->id);
            memcpy(tmp_name, srv->name, 256);
            free(srv);

            services_busy = false;
            np_log(NP_DEBUG, "services_unregister: service '%s'[%ld] has been unregistered. Services amount: %ld", tmp_name, service_id, services->__items_size);
            return NP_SERVICE_SUCCESS;
        }
    }

    services_busy = false;
    return NP_SERVICE_NOT_FOUND;
}

void np_intr_services_unregister_conn(struct NP_API_Conn *conn)
{struct NP_Service *srv; uint64_t service_id; char tmp_name[256];
    // wait until the services manager would not be busy
    while (services_busy);
    services_busy = true;

    // check that service with the same id exists
    np_list_foreach(services, item)
    {
        srv = (struct NP_Service*)(*item)->value;
        if (srv->conn_ptr == conn)
        {
            np_list_remove(services, (*item)->id);
            service_id = srv->service_id;
            memcpy(tmp_name, srv->name, 256);
            free(srv);

            np_log(NP_DEBUG, "services_unregister_conn: service '%s'[%ld] has been unregistered. Services amount: %ld", tmp_name, service_id, services->__items_size);
        }
    }

    services_busy = false;
}

int32_t np_intr_services_server_accept_event(struct NP_API_Conn *conn, NP_PACKET_SERVICE_EVENT *packet)
{struct NP_Service *srv; NP_Service_Event event;
    // wait until the services manager would not be busy
    while (services_busy);
    services_busy = true;

    // trying to find the service
    np_list_foreach(services, item)
    {
        srv = (struct NP_Service*)(*item)->value;
        if (srv->service_id == packet->service_id)
        {
            break;
        }
        srv = NULL;
    }

    if (srv == NULL)
    {
        return NP_SERVICE_NOT_FOUND;
    }

    event.source = packet->service_id_source;
    event.type = packet->event;
    event.value = packet->message;
    np_intr_api_protocol_services_event_send(srv->conn_ptr, packet->service_id, &event);

    services_busy = false;
    return NP_SERVICE_SUCCESS;
}

