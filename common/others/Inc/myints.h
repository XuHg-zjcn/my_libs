#ifndef INC_STEP_MOTOR_H_
#define INC_STEP_MOTOR_H_

#include <stdint.h>

#define USE_IXX

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#ifdef USE_IXX
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
#endif

#ifdef USE_FXX
typedef float f32;
typedef double f64;
#endif

#define SIGN(x)         ((x)==0?0:((x)>0?1:-1))

//result sign same of `a`
#define ZERO_DIV(a, b)  ((a)/(b))
#define FLOOR_DIV(a, b) (ZERO_DIV(a+(a<0), b)-(a<0))
#define CEIL_DIV(a, b)  (ZERO_DIV(a-(a>0), b)+(a>0))
#define ROUND_DIV(a, b) FLOOR_DIV((a)+(b)/2, b)

#define F32_SIGN(x)     ((*(u32*)&x)&0x80000000)
#define F32_EXP(x)      ((*(u32*)&x)&0x7f800000)
#define F32_MANT(x)     ((*(u32*)&x)&0x007fffff)
#define F32_EXP8(x)     ((i32)(F32_EXP(x)>>23)-127)

#ifdef __cplusplus
extern "C" {
#endif
int flog10(float x);
#ifdef __cplusplus
}
#endif


#endif /* INC_STEP_MOTOR_H_ */
