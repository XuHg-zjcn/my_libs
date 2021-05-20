#include "c_stream.hpp"
#include <stdarg.h>

void StrStream::printf(const char *fmt, ...)
{
    uint32_t len;
    va_list list;
    va_start(list, fmt);
    len = vsnprintf((char*)buff, b_size, fmt, list);
    va_end(list);
    st.send(buff, len);
}

void StrStream::scanf(const char *fmt, uint32_t len, ...)
{
    st.recv(buff, len);
    va_list list;
    va_start(list, len);
    vsscanf((char*)buff, fmt, list);
    va_end(list);
}

StrStream& StrStream::operator<<(const int i)
{
    printf("%d", i);
    return *this;
}

StrStream& StrStream::operator<<(const char* s)
{
    printf("%s", s);
    return *this;
}
