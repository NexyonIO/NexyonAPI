#pragma once

#include <stdint.h>

// size in bytes
#define NP_PACKET_HEADER_SIZE 12
#define NP_PACKET_HEADER uint32_t magic;     \
                uint32_t packet_id; \
                uint32_t buffer_size

// note: max packet size is 512 bytes
typedef struct
{
    NP_PACKET_HEADER;
    uint8_t buffer[512];
} NP_PACKET;

typedef struct
{
    NP_PACKET_HEADER;
    char version[16];
    uint32_t version_id;
} NP_PACKET_HANDSHAKE;

typedef struct
{
    NP_PACKET_HEADER;
    uint64_t service_id;
    int32_t result;
    uint8_t flags;
    uint32_t name_len;
    char name[256]; // max service name length is 256 bytes
} NP_PACKET_SERVICE_REGISTER;

typedef struct
{
    NP_PACKET_HEADER;
    uint64_t service_id;
    int32_t result;
} NP_PACKET_SERVICE_UNREGISTER;

