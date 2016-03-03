
#include "IOPrintf.h"

#include <stdarg.h>
#include <stdio.h>

// ########################################################################

std::string ioprintf(const char* format, ...)
{
    char message[512];

    va_list ap;
    va_start(ap, format);
    vsnprintf(message, sizeof(message), format, ap);
    va_end(ap);

    return std::string(message);
}

