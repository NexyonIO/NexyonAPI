#include "lib/internals/api/api.h"
#include "lib/internals/api/api_protocol.h"
#include "lib/internals/api/api_packets.h"

#include "napi/core.h"

#include <unistd.h>
#include <string.h>

int32_t np_intr_api_protocol_handshake_send(struct NP_API_Conn *conn)
{NP_PACKET_HANDSHAKE packet; int32_t ret;
    np_log(NP_DEBUG, "api_protocol_handshake_send: attempting to handshake.", conn->pool_id);

    NP_API_INIT_PACKET(packet, NP_PACKET_ID_HANDSHAKE);
    
    // send the packet
    memcpy(packet.version, conn->version, 16);
    packet.version_id = conn->version_id;
    NP_API_SEND_PACKET(conn, &packet);

    // wait for server response
    ret = np_intr_api_protocol_poll(conn);
    NP_API_RETURN(ret);

    // fetch and check the packet
    ret = np_intr_api_protocol_packet_fetch(conn, (NP_PACKET*)&packet);
    NP_API_RETURN(ret);
    if (packet.magic != NP_API_MAGIC)
    {
        return NP_PROTOCOL_INVALID_MAGIC;
    }
    else if (packet.packet_id != NP_PACKET_ID_HANDSHAKE)
    {
        return NP_PROTOCOL_INVALID_PACKET_ID;
    }

    np_log(NP_DEBUG, "handshake_send: handshake success.");
    conn->connected = true;
    return NP_PROTOCOL_SUCCESS;
}

int32_t np_intr_api_protocol_handshake_recv(struct NP_API_Conn *conn, NP_PACKET_HANDSHAKE *packet)
{int32_t ret;
    if (packet->magic != NP_API_MAGIC)
    {
        return NP_PROTOCOL_INVALID_MAGIC;
    }
    else if (packet->packet_id != NP_PACKET_ID_HANDSHAKE)
    {
        return NP_PROTOCOL_INVALID_PACKET_ID;
    }

    memcpy(conn->version, packet->version, 16);
    conn->version_id = packet->version_id;

    // send response to the server
    conn->connected = true;
    NP_API_SEND_PACKET(conn, packet);
    
    np_log(NP_DEBUG, "handshake_recv[%d]: handshake success, VERSION: %s, VERSION_ID: 0x%x", conn->pool_id, conn->version, conn->version_id);
    return NP_PROTOCOL_SUCCESS;
}
