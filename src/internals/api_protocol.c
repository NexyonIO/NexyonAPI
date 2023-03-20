#include "internals/api_packets.h"
#include "internals/core.h"
#include "napi/core.h"

#include "internals/api_protocol.h"

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <sys/socket.h>

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

int32_t np_intr_api_protocol_handshake_send(struct NP_API_Conn *conn)
{NP_PACKET_HANDSHAKE packet; int32_t ret;
    np_log(NP_DEBUG, "api_protocol_handshake_send: attempting to handshake.", conn->pool_id);

    NP_API_INIT_PACKET(packet, NP_PACKET_ID_HANDSHAKE, 20);
    
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

