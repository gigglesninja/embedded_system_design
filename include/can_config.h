#ifndef CAN_CONFIG_H
#define	CAN_CONFIG_H

#include "../../../pic24lib_all/lib/include/pic24_all.h"
#include "../../../pic24lib_all/lib/include/pic24_ecan.h"

#define NUM_TX_BUFS  1
#define NUM_BUFS    8

#define MSG_ID 0x7A0

__eds__ ECANMSG msgBuf[NUM_BUFS] __attribute__((space(dma),aligned(NUM_BUFS*16),eds));

void configDMA0(void) {
  DMAPWC = 0;
  _DMA0IF = 0;
  DMA0PAD = (unsigned int) &C1TXD;
  DMA0REQ = DMA_IRQ_ECAN1TX;
  DMA0STAL = (unsigned long int) &msgBuf;
  DMA0STAH = (unsigned long int) &msgBuf;
  DMA0CNT =   7;
  DMA0CON =
    (DMA_MODULE_ON |
     DMA_SIZE_WORD |
     DMA_DIR_WRITE_PERIPHERAL |
     DMA_INTERRUPT_FULL |
     DMA_NULLW_OFF |
     DMA_AMODE_PERIPHERAL_INDIRECT |
     DMA_MODE_CONTINUOUS);
}

void configDMA1(void) {
  _DMA1IF = 0;
  DMA1PAD = (unsigned int) &C1RXD;
  DMA1REQ = DMA_IRQ_ECAN1RX;
  DMA1STAL = (unsigned long int) &msgBuf;
  DMA1STAH = (unsigned long int) &msgBuf;
  DMA1CNT =  7;
  DMA1CON =  
    (DMA_MODULE_ON |
     DMA_SIZE_WORD |
     DMA_DIR_READ_PERIPHERAL |
     DMA_INTERRUPT_FULL |
     DMA_NULLW_OFF |
     DMA_AMODE_PERIPHERAL_INDIRECT |
     DMA_MODE_CONTINUOUS);
}

#define RX_BUFFER_ID  15  

void configECAN1() {
  uint8_t u8_i;
  CHANGE_MODE_ECAN1(ECAN_MODE_CONFIGURE);
  configBaudECAN1();
  C1FCTRL = ECAN_FIFO_START_AREA_1 + ECAN_DMA_BUF_SIZE_8;
  configRxFilterECAN1(0, MSG_ID, 0, RX_BUFFER_ID, 0);
  configRxMaskECAN1(0, 0x000, 0, 0);  //check all but last two bits
  clrRxFullOvfFlagsECAN1(); 

  for (u8_i = 0; u8_i<8; u8_i++) {
    if (u8_i < NUM_TX_BUFS)
      configTxRxBufferECAN1(u8_i,ECAN_TX_BUFF,3);
    else
      configTxRxBufferECAN1(u8_i,ECAN_RX_BUFF,3);
  }
  configDMA0();
  configDMA1();

  CONFIG_C1TX_TO_RP(102);
  RPINR26bits.C1RXR = 101;

  CHANGE_MODE_ECAN1(ECAN_MODE_NORMAL);
}

uint32_t rrot32(uint32_t u32_x) {
  if (u32_x & 0x1) {
    u32_x = u32_x >> 1;
    u32_x = u32_x | 0x80000000;
  } else u32_x = u32_x >> 1;
  return u32_x;
}

#endif