#ifndef INC_STEP_MOTOR_H_
#define INC_STEP_MOTOR_H_

#include <stdint.h>

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

typedef struct{
    int H4;
    int L4;
}i4x2;

typedef struct{
    int a0;
    int a1;
    int a2;
    int a3;
}i2x4;

typedef struct{
    unsigned int H4;
    unsigned int L4;
}u4x2;

typedef struct{
    unsigned int a0;
    unsigned int a1;
    unsigned int a2;
    unsigned int a3;
}u2x4;

//1bit please use bitband

#endif /* INC_STEP_MOTOR_H_ */
