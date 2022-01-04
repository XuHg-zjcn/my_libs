#ifndef __PRINTF_XXX_H__
#define __PRINTF_XXX_H__

#include <stdint.h>
//#include <stdio.h>  //请改为所需的头文件

//#define PUTC(c) putchar(c) //请改为所需的发送字符函数

#define printf_u(x)     printf_fmu(' ', 0, x)
#define printf_d(x)     printf_fmd(' ', 0, x)
#define printf_mu(m, x) printf_fmu(' ', m, x)
#define printf_md(m, x) printf_fmd(' ', m, x)
#define printf_c(c)     PUTC(c)

void printf_fmu(const char fill, uint8_t m, uint32_t x);
void printf_fmd(const char fill, uint8_t m, int32_t x);
void printf_s(char* s);
void printf_rep(const char c, uint8_t n);

#endif /* __PRINTF_XXX_H__ */
