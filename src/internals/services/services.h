#pragma once

#include "napi/services.h"

#include "internals/api/api_packets.h"
#include "internals/api/api.h"

// server side
int32_t np_intr_services_register(struct NP_API_Conn *conn, uint64_t service_id, const char *name, uint8_t flags);
int32_t np_intr_services_unregister(uint64_t service_id);
int32_t np_intr_services_server_accept_event(struct NP_API_Conn *conn, NP_PACKET_SERVICE_EVENT *packet);
void np_intr_services_unregister_conn(struct NP_API_Conn *conn);
void np_intr_services_server_init();
void np_intr_services_server_destroy();

// client side
void np_intr_services_accept_event(uint64_t service_id, NP_Service_Event *event);
void np_intr_services_client_destroy();
void np_intr_services_client_init();

