#ifndef INC_STEP_MOTOR_H_
#define INC_STEP_MOTOR_H_

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint32_t u64;

#ifdef USE_IXX
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int32_t i64;
#endif

#ifdef USE_FXX
typedef float f32;
typedef double f64;
#endif

#define FLOOR_DIV(a, b) (a/b)
#define ROUND_DIV(a, b) ((a+b/2)/b)
#define CEIL_DIV(a, b)  ((a+b-1)/b)

#endif /* INC_STEP_MOTOR_H_ */
