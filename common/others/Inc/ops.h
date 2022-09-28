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

#define mean(s, n)      (s/n)
#define std(s, s2, n)   ((s2 - s1*s1/n)/n)

#define CAST(t, x) (*(t*)(&(x)))

#endif
