#include "internals/api/api.h"
#include "internals/api/api_protocol.h"
#include "internals/api/api_packets.h"
#include "internals/services/services.h"

#include "napi/core.h"
#include "napi/services.h"

#include <error.h>
#include <unistd.h>
#include <string.h>

int32_t np_intr_api_protocol_services_register_send(struct NP_API_Conn *conn, uint64_t service_id, const char *name, uint8_t flags)
{NP_PACKET_SERVICE_REGISTER packet; int32_t ret;
    NP_API_INIT_PACKET(packet, NP_PACKET_ID_SERVICE_REGISTER);

    // telling the server that we want to register a service
    memset(packet.name, '\0', 256);
    memcpy(packet.name, name, strlen(name));
    packet.flags = flags;
    packet.service_id = service_id;
    packet.result = NP_SERVICE_INTERNAL_ERROR;
    NP_API_SEND_PACKET(conn, &packet);

    // wait for server response
    ret = np_intr_api_protocol_poll(conn);
    NP_API_RETURN(ret);

    // read the server response
    ret = np_intr_api_protocol_packet_fetch(conn, (NP_PACKET*)&packet);
    NP_API_RETURN(ret);
    if (packet.magic != NP_API_MAGIC)
    {
        return NP_PROTOCOL_INVALID_MAGIC;
    }
    else if (packet.packet_id != NP_PACKET_ID_SERVICE_REGISTER)
    {
        return NP_PROTOCOL_INVALID_PACKET_ID;
    }
    
    return packet.result;
}

int32_t np_intr_api_protocol_services_register_recv(struct NP_API_Conn *conn, NP_PACKET_SERVICE_REGISTER *packet)
{
    if (packet->magic != NP_API_MAGIC)
    {
        return NP_PROTOCOL_INVALID_MAGIC;
    }
    else if (packet->packet_id != NP_PACKET_ID_SERVICE_REGISTER)
    {
        return NP_PROTOCOL_INVALID_PACKET_ID;
    }

    // attempt to register the service and send response to the client
    packet->result = np_intr_services_register(conn, packet->service_id, packet->name, packet->flags);
    NP_API_SEND_PACKET(conn, packet);

    return NP_PROTOCOL_SUCCESS;
}

int32_t np_intr_api_protocol_services_unregister_send(struct NP_API_Conn *conn, uint64_t service_id)
{NP_PACKET_SERVICE_UNREGISTER packet; int32_t ret;
    NP_API_INIT_PACKET(packet, NP_PACKET_ID_SERVICE_UNREGISTER);

    // telling the server that we want to register a service
    packet.service_id = service_id;
    packet.result = NP_SERVICE_INTERNAL_ERROR;
    NP_API_SEND_PACKET(conn, &packet);

    // wait for server response
    ret = np_intr_api_protocol_poll(conn);
    NP_API_RETURN(ret);

    // read the server response
    ret = np_intr_api_protocol_packet_fetch(conn, (NP_PACKET*)&packet);
    NP_API_RETURN(ret);
    if (packet.magic != NP_API_MAGIC)
    {
        return NP_PROTOCOL_INVALID_MAGIC;
    }
    else if (packet.packet_id != NP_PACKET_ID_SERVICE_UNREGISTER)
    {
        return NP_PROTOCOL_INVALID_PACKET_ID;
    }

    return packet.result;
}

int32_t np_intr_api_protocol_services_unregister_recv(struct NP_API_Conn *conn, NP_PACKET_SERVICE_UNREGISTER *packet)
{
    if (packet->magic != NP_API_MAGIC)
    {
        return NP_PROTOCOL_INVALID_MAGIC;
    }
    else if (packet->packet_id != NP_PACKET_ID_SERVICE_UNREGISTER)
    {
        return NP_PROTOCOL_INVALID_PACKET_ID;
    }

    // attempt to register the service and send response to the client
    packet->result = np_intr_services_unregister(packet->service_id);
    NP_API_SEND_PACKET(conn, packet);

    return NP_PROTOCOL_SUCCESS;
}

int32_t np_intr_api_protocol_services_event_send(struct NP_API_Conn *conn, uint64_t service_id, NP_Service_Event *event)
{NP_PACKET_SERVICE_EVENT packet; int32_t ret;
    NP_API_INIT_PACKET(packet, NP_PACKET_ID_SERVICE_EVENT);

    // telling the server that we want to register a service
    packet.service_id = service_id;
    packet.service_id_source = event->source;
    packet.event = event->type;
    packet.buffer_size = sizeof(packet) - NP_PACKET_HEADER_SIZE - sizeof(char*);
    memset(packet.message, '\0', 512);
    memcpy(packet.message, event->value, strlen(event->value));
 
    // send the event
    ret = NP_API_SEND_PACKET(conn, &packet);

    if (ret <= 0)
    {
        np_log(NP_WARN, "protocol_services_event_send: failed to send event");
        return NP_PROTOCOL_UNKNOWN_ERROR;
    }

    return NP_PROTOCOL_SUCCESS;
}

int32_t np_intr_api_protocol_services_event_recv(struct NP_API_Conn *conn, NP_PACKET_SERVICE_EVENT *packet)
{int32_t ret; NP_Service_Event event;
    if (conn->side)
    { // server side connection
        if (packet->magic != NP_API_MAGIC)
        {
            return NP_PROTOCOL_INVALID_MAGIC;
        }
        else if (packet->packet_id != NP_PACKET_ID_SERVICE_EVENT)
        {
            return NP_PROTOCOL_INVALID_PACKET_ID;
        }
     
        np_intr_services_server_accept_event(conn, packet);
    }
    else
    {
        event.source = packet->service_id_source;
        event.type = packet->event;
        event.value = packet->message;

        np_intr_services_accept_event(packet->service_id, &event);
    }

    return NP_PROTOCOL_SUCCESS;
}

