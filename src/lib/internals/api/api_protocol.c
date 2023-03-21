#include "napi/core.h"

#include "lib/internals/api/api_packets.h"
#include "lib/internals/api/api_protocol.h"
#include "lib/internals/core.h"

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <sys/socket.h>

int32_t np_intr_api_protocol_client_handle(struct NP_API_Conn *conn, int32_t value)
{
    if (value >> 8 == NP_API_RESPONSES_PREFIX && value != NP_PROTOCOL_SUCCESS)
    {
        np_log(NP_ERROR, "protocol_client_handle: failed to fetch packet (%s)", np_return_value_meaning(value));
        np_destroy();
        return value;  
    }

    return NP_PROTOCOL_SUCCESS;
}

int32_t np_intr_api_protocol_execute(struct NP_API_Conn *conn)
{NP_PACKET packet; int32_t ret;
    // fetch the packet contents and execute it
    // note: do not use np_intr_api_protocol_poll, so the
    // function won't fail because of timeout
    // (we need it, because the client can send packet at any time)
    ret = np_intr_api_protocol_packet_fetch(conn, (NP_PACKET*)&packet);
    NP_API_RETURN(ret);

    switch (packet.packet_id)
    {
    case NP_PACKET_ID_HANDSHAKE:
        { ret = np_intr_api_protocol_handshake_recv(conn, (NP_PACKET_HANDSHAKE*)&packet); break; }
    case NP_PACKET_ID_SERVICE_REGISTER:
        { ret = np_intr_api_protocol_services_register_recv(conn, (NP_PACKET_SERVICE_REGISTER*)&packet); break; }
    case NP_PACKET_ID_SERVICE_UNREGISTER:
        { ret = np_intr_api_protocol_services_unregister_recv(conn, (NP_PACKET_SERVICE_UNREGISTER*)&packet); break; }
    case NP_PACKET_ID_SERVICE_EVENT:
        { ret = np_intr_api_protocol_services_event_recv(conn, (NP_PACKET_SERVICE_EVENT*)&packet); break; }
    }
    NP_API_RETURN(ret);

    return ret;
}

int32_t np_intr_api_protocol_poll(struct NP_API_Conn *conn)
{struct pollfd fd; int32_t ret;

    fd.fd = conn->fd; 
    fd.events = POLLIN;
    ret = poll(&fd, 1, NP_API_TIMEOUT);

    switch (ret)
    {
    case -1:
        { conn->connected = false; return NP_PROTOCOL_UNKNOWN_ERROR; } 
    case 0:
        { conn->connected = false; return NP_PROTOCOL_TIMEOUT; }
    }

    return NP_PROTOCOL_SUCCESS;
}

int32_t np_intr_api_protocol_packet_fetch(struct NP_API_Conn *conn, NP_PACKET *packet)
{int32_t ret;
    ret = NP_API_FETCH0_PACKET(conn, packet);
    if (ret <= 0) goto err;
    ret = NP_API_FETCH1_PACKET(conn, packet);
    if (ret <= 0) goto err;

    if (packet->magic == NP_API_MAGIC) goto end;
err:
    return NP_PROTOCOL_DISCONNECT;
end:
    return NP_PROTOCOL_SUCCESS;
}

