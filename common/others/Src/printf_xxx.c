#include "printf_xxx.h"
#ifdef __PRINTF_XXX_H__

//保存数字字符串的堆栈
//足够保存u32的最大值，有符号整数不用保存负号
#ifdef __SDCC
__xdata char buf[10];
#else
char buf[10];
#endif

char* p;

void printf_fmu(const char fill, uint8_t m, uint32_t x)
{
    p=buf;
    while(x){
        *p++='0'+(x%10);
        x/=10;
    }
    if(m>p-buf){
        m-=p-buf;
        while(m--){
            PUTC(fill);
        }
    }
    while(p>buf){
        PUTC(*--p);
    }
}

void printf_fmd(const char fill, uint8_t m, int32_t x)
{
    if(x<0){
        PUTC('-');
        x = -x;
    }
    printf_fmu(fill, m, x);
}

void printf_s(char* s)
{
    while(*s){
        PUTC(*s++);
    }
}

void printf_rep(const char c, uint8_t n)
{
    while(n--){
        PUTC(c);
    }
}

#endif /* __PRINTF_XXX_H__ */
