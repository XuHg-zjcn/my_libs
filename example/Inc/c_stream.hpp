typedef enum{
TransBlocking = 0,
TransInterrupt,
}TransType;

typedef enum{
Wait_error = 0;
Wait_while,
Wait_rtos
}WaitType;

typedef struct{
    TransType trans:1;
    bool DMA_use:1;  //is use DMA?
    bool DMA_cfg:1;  //if DMA config other channel, auto reconfig
    bool useOther:1; //True:use mode in this->trans, False:wait
    WaitType wait:2; //wait type if useOther=False
}TransTypeStru;

class Stream{
public:
    send(uint8_t* data, uint32_t size);
    recv(uint8_t* data, uint32_t size);
}

class StrStream{
private:
    Stream& st;
    uint8_t* buff;
    uint32_t b_size;
public:
    StrStream(Stream &st);
    printf(const char *fmt, ...);
    scanf(const char *fmt, ...);
    StrStream& operator<<(const int i);
    StrStream& operator<<(const char* c);
}