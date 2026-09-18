#include <stdio.h>
#include <stdarg.h>

void dPrintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

int dSprintf(char *buffer, size_t bufferSize, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    int len = vsnprintf(buffer, bufferSize, format, args);
    va_end(args);

    return (len);
}
