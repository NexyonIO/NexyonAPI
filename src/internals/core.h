#pragma once

#include <stdint.h>
#include <stdbool.h>

// the version should contains 16 bytes long string and 4 bytes long identifier
#define NP_VERSION "0.1b\0\0\0\0\0\0\0\0\0\0\0\0"
// actually, this is used to determine the API version that both server and client should use
// todo: change the nameing
#define NP_VERSION_ID 0x01

enum NP_INTR_PLATFORM_KEYS
{
    NP_INTR_KERNEL_INFO,
    NP_INTR_HARDWARE_INFO,
};

bool np_intr_ansi_support();
const char *np_intr_platform_info(uint8_t type);

void np_intr_server_handler_block();
void np_intr_server_handler_unblock();
void np_intr_server_handler(void *unused);
