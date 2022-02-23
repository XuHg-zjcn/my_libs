#include "c_adc.hpp"

CDMA_Channel *const DMA_CH_ADC = \
  (CDMA_Channel *)DMA1_Channel1_BASE;

void C_ADC::Init()
{
  CTLR2.ADON = true;
}

void C_ADC::set_tSMP(CADC_CHx channel, CADC_tSMP smp)
{
  if(channel < 10){
    MODIFY_REG(SAMPTR1, CADC_tSMP_Msk<<(3*channel), ((u32)smp)<<(3*channel));
  }else{
    channel = (CADC_CHx)(channel - 10);
    MODIFY_REG(SAMPTR2, CADC_tSMP_Msk<<(3*channel), ((u32)smp)<<(3*channel));
  }
}

u16 C_ADC::read_channel(CADC_CHx channel, CADC_tSMP smp)
{
  WRITE_REG(ISQR, channel);
  CTLR2.IEXTSEL = IT_SWSTART;
  set_tSMP(channel, smp);
  CTLR2.ISWSTART = true;
  while(STATR.IEOC == false);
  STATR.IEOC = false;
  return IDATAx[0];
}

void C_ADC::Init_DMA()
{
  CTLR2.DMA = true;
  CDMA_CFGR tmp = {
    .EN = false,
    .TCIE = false,
    .HTIE = false,
    .TEIE = false,
    .DIR = DMA_P2M,
    .CIRC = false,
    .PINC = false,
    .MINC = true,
    .PSIZE = DMA_SIZE_16bit,
    .MSIZE = DMA_SIZE_16bit,
    .PL = DMA_PL_Mid,
    .M2M = false
  };
  SET_BIT(DMA1->INTFCR, DMA_CGIF1 | DMA_CTCIF1 | DMA_CHTIF1 | DMA_CTEIF1);
  CAST(u32, DMA_CH_ADC->CFGR) = CAST(u32, tmp);
  DMA_CH_ADC->PADDR = &RDATAR;
}

void C_ADC::DMA_once(u16 *buff, u16 Nsamp)
{
  DMA_CH_ADC->MADDR = (void *)buff;
  DMA_CH_ADC->CNTR = Nsamp;
  DMA_CH_ADC->CFGR.EN = true;
}
