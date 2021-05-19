#include <stdio.h>

StrStream::printf(const char *fmt, ...)
{
    uint32_t len;
    va_list list;
    va_start(list, fmt);
    len = vsnprintf(buff, b_size, fmt, list);
    va_end(list);
    send(buff, len);
}

StrStream::scanf(const char *fmt, uint32_t len, ...)
{
    recv(buff, len);
    va_list list;
    va_start(list, fmt);
    vsscanf(buff, fmt, list);
    va_end(list);
}

StrStream& operator<<(const int i)
{
    printf("%d", i);
}

StrStream& operator<<(const char* s)
{
    printf("%s", s);
}
