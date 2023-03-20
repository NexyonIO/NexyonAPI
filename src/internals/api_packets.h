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

