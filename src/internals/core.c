#include "core.h"

#include <sys/utsname.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>

bool np_intr_ansi_support()
{
    return isatty(fileno(stdout));
}

const char *np_intr_platform_info(uint8_t type)
{
#define PLATFORM_INFO_LEN 256
    static char platform_info[PLATFORM_INFO_LEN];
    struct utsname buffer;

    if (type == NP_INTR_KERNEL_INFO)
    {
        if (uname(&buffer) < 0)
        {
            return "linux";
        }
        
        memset(platform_info, '\0', PLATFORM_INFO_LEN);
        sprintf(platform_info, "%s %s (%s)", buffer.sysname, buffer.release, buffer.machine);
    }
    else
    {
        memcpy(platform_info, "not implemented\0", 16);
    }

    return platform_info;
}

