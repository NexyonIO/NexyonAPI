#pragma once

#include "napi/services.h"

#include "lib/internals/api/api_packets.h"
#include "lib/internals/api/api.h"

// stands for 'napi' in ascii
#define NP_API_MAGIC 0x6E617069

#define NP_API_RETURN(code) do { int32_t __ret = code; if (__ret != NP_PROTOCOL_SUCCESS) return __ret; } while(0)
#define NP_API_SEND_PACKET(conn, packet) send(conn->fd, (uint8_t*)packet, ((packet)->buffer_size) + NP_PACKET_HEADER_SIZE, MSG_NOSIGNAL)
#define NP_API_FETCH0_PACKET(conn, packet) read(conn->fd, (uint8_t*)packet, NP_PACKET_HEADER_SIZE)
#define NP_API_FETCH1_PACKET(conn, packet) read(conn->fd, (uint8_t*)packet+NP_PACKET_HEADER_SIZE, packet->buffer_size)
#define NP_API_INIT_PACKET(packet, id) do { packet.magic = NP_API_MAGIC; packet.packet_id = id; packet.buffer_size = sizeof(packet)-NP_PACKET_HEADER_SIZE; } while (0)
#define NP_API_INIT_PACKET_NLEN(packet, id, len) do { packet.magic = NP_API_MAGIC; packet.packet_id = id; packet.buffer_size = sizeof(packet)+len-NP_PACKET_HEADER_SIZE; } while (0)

#define NP_API_RESPONSES_PREFIX 0xfa
// prefix - fa
enum NP_INTR_API_PROTOCOL_RESULTS
{
    NP_PROTOCOL_SUCCESS = 0xfa01,
    NP_PROTOCOL_INVALID_MAGIC = 0xfa02,
    NP_PROTOCOL_INCOMPATIBLE_VERSION = 0xfa03,
    NP_PROTOCOL_UNKNOWN_ERROR = 0xfa04,
    NP_PROTOCOL_TIMEOUT = 0xfa05,
    NP_PROTOCOL_INVALID_PACKET_ID = 0xfa06,
    NP_PROTOCOL_DISCONNECT = 0xfa07,
};

enum NP_INTR_API_PACKETS
{
    NP_PACKET_ID_HANDSHAKE = 0x01,
    NP_PACKET_ID_SERVICE_REGISTER = 0x02,
    NP_PACKET_ID_SERVICE_UNREGISTER = 0x03,
    NP_PACKET_ID_SERVICE_EVENT = 0x04,
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

Services register (serverbound):
    12-19: service id
    20-23: result
    24-25: flags
    26-...: name

Services unregister (serverbound):
    12-19: service id
    20-23: result

Services event recv (clientbound):
Services event send (serverbound):
    12-19:  service id
    20-31:  service id source
    32-35:  event
    36-...: message
*/

int32_t np_intr_api_protocol_client_handle(struct NP_API_Conn *conn, int32_t value);
int32_t np_intr_api_protocol_packet_fetch(struct NP_API_Conn *conn, NP_PACKET *packet);
int32_t np_intr_api_protocol_poll(struct NP_API_Conn *conn);
int32_t np_intr_api_protocol_execute(struct NP_API_Conn *conn);

// handshake
int32_t np_intr_api_protocol_handshake_send(struct NP_API_Conn *conn);
int32_t np_intr_api_protocol_handshake_recv(struct NP_API_Conn *conn, NP_PACKET_HANDSHAKE *packet);

// services
int32_t np_intr_api_protocol_services_register_send(struct NP_API_Conn *conn, uint64_t service_id, const char *name, uint8_t flags);
int32_t np_intr_api_protocol_services_register_recv(struct NP_API_Conn *conn, NP_PACKET_SERVICE_REGISTER *packet);

int32_t np_intr_api_protocol_services_unregister_send(struct NP_API_Conn *conn, uint64_t service_id);
int32_t np_intr_api_protocol_services_unregister_recv(struct NP_API_Conn *conn, NP_PACKET_SERVICE_UNREGISTER *packet);

int32_t np_intr_api_protocol_services_event_send(struct NP_API_Conn *conn, uint64_t service_id, NP_Service_Event *event);
int32_t np_intr_api_protocol_services_event_recv(struct NP_API_Conn *conn, NP_PACKET_SERVICE_EVENT *packet);

