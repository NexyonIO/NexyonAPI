#include "napi/core.h"

#include "lib/internals/ansi.h"
#include "lib/internals/core.h"

#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

bool wait = false;
static const char *log_prefixes[] = {
    [ NP_INFO ] = "%sINFO [(%s) %s:%d] %s%s",
    [ NP_DEBUG ] = "%sDEBUG [(%s) %s:%d] %s%s",
    [ NP_WARN ] = "%sWARNING [(%s) %s:%d] %s%s",
    [ NP_ERROR ] = "%sERROR [(%s) %s:%d] %s%s",
    [ NP_PANIC ] = "%sPANIC [(%s) %s:%d] %s%s",
};

const char *np_log_prefix(const char *prefix)
{
    static const char *log_prefix = "napi";
    if (prefix != NULL)
    {
        log_prefix = prefix;
    }

    return log_prefix;
}

int32_t __np_log(const char *file, uint32_t line, const char *func, uint8_t level, const char *fmt, ...)
{
    //while (wait);
    wait = true;

    if (level >= NP_LOG_LEVELS_COUNT)
    {
        wait = false;
        assert(level < NP_LOG_LEVELS_COUNT);
    }
    
    FILE *out = stdout;
    int32_t len = 0;
    const char *color = "\0";
    const char *prefix_color = "\0";

    // set proper output
    if (level == NP_PANIC || level == NP_ERROR)
    {
        out = stderr;
    }

    // check if the terminal supports ansi
    if (np_intr_ansi_support())
    {
        prefix_color = ANSI_GRN;
        
        switch (level)
        {
        case NP_INFO:
            { color = ANSI_CYN; break; }
        case NP_WARN:
            { color = ANSI_YEL; break; }
        case NP_DEBUG:
            { color = ANSI_WHT; break; }
        case NP_ERROR:
            { color = ANSI_RED; break; }
        case NP_PANIC:
            { color = ANSI_RED; break; }
        }
    }

    // print log message
    va_list args;
    va_start(args, fmt);
    len += fprintf(out, log_prefixes[level], color, file, func, line, prefix_color, np_log_prefix(NULL));
    if (np_intr_ansi_support())
        len += fprintf(out, "%s: ", ANSI_COLOR_RESET);
    len += vfprintf(out, fmt, args);
    if (np_intr_ansi_support())
        len += fprintf(out, ANSI_COLOR_RESET);
    ++len; putc('\n', out);
    va_end(args);

    // send abort signal on panic log level
    if (level == NP_PANIC)
    {
        wait = false;
        raise(SIGABRT);
    }

    wait = false;
    return len;
}

