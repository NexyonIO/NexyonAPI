#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
#define NP_LIB extern "C"
#else
#define NP_LIB extern
#endif

#define np_log(level, args...) __np_log(__FILE__, __LINE__, __func__, level, args)

#define NP_LOG_LEVELS_COUNT 5
enum LOG_LEVELS
{
    NP_INFO = 0, NP_WARN, NP_ERROR, NP_DEBUG, NP_PANIC
};

NP_LIB int32_t __np_log(const char *file, uint32_t line, const char *func, uint8_t level, const char *fmt, ...);
NP_LIB const char *np_log_prefix(const char *prefix);
NP_LIB int32_t np_main(int32_t argc, char **argv);
NP_LIB int32_t np_destroy();
NP_LIB bool np_alive();
NP_LIB const char *np_return_value_meaning(int32_t value);

