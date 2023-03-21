#pragma once

#include "napi/services.h"

#include "lib/internals/api/api_protocol.h"

static const char *NP_RESULT_STRINGS[] =
{
    // protocol
    [ NP_PROTOCOL_SUCCESS ] = "api: success",
    [ NP_PROTOCOL_INVALID_MAGIC ] = "api: invalid magic",
    [ NP_PROTOCOL_INVALID_PACKET_ID ] = "api: invalid packet id",
    [ NP_PROTOCOL_INCOMPATIBLE_VERSION ] = "api: incompatible version",
    [ NP_PROTOCOL_UNKNOWN_ERROR ] = "api: unknown error",
    [ NP_PROTOCOL_TIMEOUT ] = "api: timeout",
    [ NP_PROTOCOL_DISCONNECT ] = "api: disconnect",

    // services
    [ NP_SERVICE_SUCCESS ] = "services: success",
    [ NP_SERVICE_OFFLINE ] = "services: offline",
    [ NP_SERVICE_ALREADY_EXISTS ] = "services: already exists",
    [ NP_SERVICE_INVALID_ID ] = "services: invalid id",
    [ NP_SERVICE_INCORRECT_NAME ] = "services: incorrect name",
    [ NP_SERVICE_INTERNAL_ERROR ] = "services: internal error",
    [ NP_SERVICE_NOT_FOUND ] = "services: not found",
};


