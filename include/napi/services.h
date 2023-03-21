#pragma once

#include "core.h"
#include <stdint.h>

#define NP_SERVICES_RESPONSES_PREFIX 0xfe
// prefix - 0xfe
enum NP_SERVICES_RESPONSE
{
    NP_SERVICE_SUCCESS = 0xfe00,
    NP_SERVICE_OFFLINE = 0xfe01,
    NP_SERVICE_ALREADY_EXISTS = 0xfe02,
    NP_SERVICE_INVALID_ID = 0xfe03,
    NP_SERVICE_INCORRECT_NAME = 0xfe04,
    NP_SERVICE_INTERNAL_ERROR = 0xfe05,
    NP_SERVICE_NOT_FOUND = 0xfe06,
};

enum NP_SERVICE_EVENTS
{
    NP_SERVICE_EVENT_MESSAGE = 0x00,
};

// Hardcoded IDs for internal napi services.
// note: service id should be greater than 0xff, because
//          any number that less is reserved.
enum NP_SERVICES
{
    NP_SERVICE_SWM = 0xff0
};

struct NP_Service_Event
{
    uint64_t source;
    int32_t type;
    void *value;
};

typedef struct NP_Service_Message NP_Service_Message;
typedef struct NP_Service_Event NP_Service_Event;

extern int32_t np_service_send_event(uint64_t service_id, NP_Service_Event *event);
// note: max service name is 256 bytes long
// todo: flags unused
extern int32_t np_service_register(uint64_t service_id, const char *name, uint8_t flags, void(*events_callback)(NP_Service_Event));
extern int32_t np_service_unregister(uint64_t service_id);

