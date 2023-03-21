#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define np_log(level, args...) __np_log(__FILE__, __LINE__, __func__, level, args)

#define NP_LOG_LEVELS_COUNT 5
enum LOG_LEVELS
{
    NP_INFO = 0, NP_WARN, NP_ERROR, NP_DEBUG, NP_PANIC
};

extern int32_t __np_log(const char *file, uint32_t line, const char *func, uint8_t level, const char *fmt, ...);

extern const char *np_log_prefix(const char *prefix);

extern int32_t np_main(int32_t argc, char **argv);
extern int32_t np_destroy();
extern bool np_alive();

extern const char *np_return_value_meaning(int32_t value);

