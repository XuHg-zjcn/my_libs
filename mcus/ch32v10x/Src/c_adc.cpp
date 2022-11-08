#include "c_adc.hpp"

CDMA_Channel *const DMA_CH_ADC = (CDMA_Channel *)DMA1_Channel1_BASE;

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
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  SET_BIT(DMA1->INTFCR, DMA_CGIF1 | DMA_CTCIF1 | DMA_CHTIF1 | DMA_CTEIF1);
  CAST(u32, DMA_CH_ADC->CFGR) = CAST(u32, tmp);
  DMA_CH_ADC->PADDR = &RDATAR;
}

void C_ADC::DMA_once(u16 *buff, u16 Nsamp)
{
  CAST(vu32, DMA_CH_ADC->CFGR) &= ~0b1;
  CAST(vu32, DMA_CH_ADC->MADDR) = (u32)buff;
  CAST(vu32, DMA_CH_ADC->CNTR) = Nsamp;
  CAST(vu32, DMA_CH_ADC->CFGR) |= 0b1;
  DMA1->INTFCR = DMA_GIF1;
}

void C_ADC::DMA_wait()
{
  while(!(DMA1->INTFR & DMA_TCIF1));
}

void C_ADC::Load_Reg_Seq(const ADC_aSamp *smps, const u32 len)
{
  if(len > 16){
    return;
  }
  u32 remain = len;
  u32 tmp;
  int i, j;
  for(i=2;i>=0;i--){
    tmp = (i==0)?(len<<20):0;
    for(j=0;(remain>0 && j<6);j++){
      tmp |= (smps->CHx)<<(j*5);
      set_tSMP(smps->CHx, smps->tSMP);
      smps++;
      remain--;
    }
    RSQRx[i] = tmp;
  }
}

void C_ADC::Load_Inj_Seq(const ADC_aSamp *smps, u32 len)
{
  if(len > 4){
    return;
  }
  u32 tmp = 0;
  for(int i=0;i<len;i++){
    tmp |= (smps->CHx)<<(i*5);
    set_tSMP(smps->CHx, smps->tSMP);
    smps++;
  }
  tmp <<= (4-len)*5;
  tmp |= (len-1)<<20;
  ISQR = tmp;
}

void C_ADC::start_inj()
{
  CTLR1.SCAN = true;
  STATR.IEOC = false;
  CTLR2.IEXTSEL = IT_SWSTART;
  CTLR2.ISWSTART = true;
  while(STATR.IEOC == false);
}
