#ifndef __C_DMA_HPP__
#define __C_DMA_HPP__

typedef enum{
  DMA_P2M = 0,
  DMA_M2P = 1
}DMA_DIR;

typedef enum{
  DMA_SIZE_8bit = 0,
  DMA_SIZE_16bit = 1,
  DMA_SIZE_32bit = 2
}DMA_SIZE;

typedef enum{
  DMA_PL_Low = 0,
  DMA_PL_Mid = 1,
  DMA_PL_High = 2,
  DMA_PL_Top = 3
}DMA_PL;

#pragma pack(1)

typedef struct{
  bool     EN:1;
  bool     TCIE:1;
  bool     HTIE:1;
  bool     TEIE:1;
  DMA_DIR  DIR:1;
  bool     CIRC:1;
  bool     PINC:1;
  bool     MINC:1;
  DMA_SIZE PSIZE:2;
  DMA_SIZE MSIZE:2;
  DMA_PL   PL:2;
  bool     M2M:1;
}CDMA_CFGR;

typedef struct{
  CDMA_CFGR CFGR;
  __IO u32 CNTR;
  __IO void *PADDR;
  __IO void *MADDR;
}CDMA_Channel;

class C_DMA{
public:
  __IO u32 INTFR;  //中断状态寄存器
  __IO u32 INTFCR; //中断标志清除寄存器
  __IO CDMA_Channel Channels[7];
};

#pragma pack()

#endif /* __C_DMA_HPP__ */
