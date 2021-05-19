#include "c_stream.hpp"

typedef enum{
    StopBit1 = 0,
    StopBit1_5,
    StopBit2
}StopBitType;

typedef enum{
    ParityEven = 0,
    ParityOdd,
    Parity0,
    Parity1
}ParityType;

typedef enum{
    WordLength_8b = 0,
    WordLength_9b
}WordLength;

class C_UART : public Stream{
public:
    void set_All(uint32_t bps, WordLength wl, StopBitType stopbit, ParityType parity);
    void set_Baud(uint32_t bps);
    void set_WordLength(WordLength wl);
    void set_StopBit(StopBitType stopbit);
    void set_Parity(ParityType parity);
}
