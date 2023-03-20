#pragma once

#include "internals/api_packets.h"
#include "internals/api.h"

// stands for 'napi' in ascii
#define NP_API_MAGIC 0x6E617069
#define NP_API_RETURN(code) do { int32_t __ret = code; if (__ret != NP_PROTOCOL_SUCCESS) return __ret; } while(0)
#define NP_API_SEND_PACKET(conn, packet) send(conn->fd, (uint8_t*)packet, ((packet)->buffer_size) + NP_PACKET_HEADER_SIZE, MSG_NOSIGNAL)
#define NP_API_FETCH0_PACKET(conn, packet) read(conn->fd, (uint8_t*)packet, NP_PACKET_HEADER_SIZE)
#define NP_API_FETCH1_PACKET(conn, packet) read(conn->fd, (uint8_t*)packet+NP_PACKET_HEADER_SIZE, packet->buffer_size)
#define NP_API_INIT_PACKET(packet, id, size) do { packet.magic = NP_API_MAGIC; packet.packet_id = id; packet.buffer_size = size; } while (0)

enum NP_INTR_API_PROTOCOL
{
    NP_PROTOCOL_SUCCESS = 0x1,
    NP_PROTOCOL_INVALID_MAGIC = 0x02,
    NP_PROTOCOL_INCOMPATIBLE_VERSION = 0x03,
    NP_PROTOCOL_UNKNOWN_ERROR = 0x04,
    NP_PROTOCOL_TIMEOUT = 0x05,
    NP_PROTOCOL_INVALID_PACKET_ID = 0x06,
    NP_PROTOCOL_DISCONNECT = 0x07,
};

enum NP_INTR_API_PACKETS
{
    NP_PACKET_ID_HANDSHAKE = 0x01,
};

static const char *PROTOCOL_RESULT_STRINGS[] =
{
    [ NP_PROTOCOL_SUCCESS ] = "success",
    [ NP_PROTOCOL_INVALID_MAGIC ] = "invalid magic",
    [ NP_PROTOCOL_INVALID_PACKET_ID ] = "invalid packet id",
    [ NP_PROTOCOL_INCOMPATIBLE_VERSION ] = "incompatible version",
    [ NP_PROTOCOL_UNKNOWN_ERROR ] = "unknown error",
    [ NP_PROTOCOL_TIMEOUT ] = "timeout",
    [ NP_PROTOCOL_DISCONNECT ] = "disconnect",
};

/*
Example:
    [Packet name] ([direction]):
        0-3:  magic (size in bytes)
        4-7:  packet id
        8-11: packet size (excluding header size)
        12-512: other data

Handshake (serverbound):
    12-27: version name
    28-31: version id
*/

int32_t np_intr_api_protocol_packet_fetch(struct NP_API_Conn *conn, NP_PACKET *packet);
int32_t np_intr_api_protocol_poll(struct NP_API_Conn *conn);
int32_t np_intr_api_protocol_execute(struct NP_API_Conn *conn);

// packets
int32_t np_intr_api_protocol_handshake_send(struct NP_API_Conn *conn);
int32_t np_intr_api_protocol_handshake_recv(struct NP_API_Conn *conn, NP_PACKET_HANDSHAKE *packet);

