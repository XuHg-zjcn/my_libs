#ifndef __OPS_H__
#define __OPS_H__


#if !defined(STM32F1) && !defined(STM32F4)

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define TOGG_BIT(REG, BIT)    ((REG) ^= (BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))

#endif

//a,b,c const, x,y,z vars

#define value_lower(x, a)   (x<a)?a:x
#define value_upper(x, b)   (x>b)?b:x
#define value_clip(x, a, b) ((x<a)?a:(x>b?b:x))

#define DIV_ZERO(a, b)  ((a)/(b))
#define DIV_FLOOR(a, b) (((a)>0^(b)>0)?((a)-(b)+((b)>0?1:-1))/(b):(a)/(b))
#define DIV_CEIL(a, b)  (((a)>0^(b)>0)?(a)/(b):((a)+(b)+((b)>0?-1:1))/(b))
#define DIV_ROUND(a, b) (((a)+(((a)>0^(b)>0)?-((b)/2):(b)/2))/(b))

#define min(a, b)       ((a)<(b)?(a):(b))
#define max(a, b)       ((a)>(b)?(a):(b))
#define mean(s, n)      (s/n)
#define std(s, s2, n)   ((s2 - s1*s1/n)/n)

#define CAST(t, x) (*(t*)(&(x)))

#endif
