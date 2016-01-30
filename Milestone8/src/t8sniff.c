#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24_spi.h"
#include "../../../pic24lib_all/lib/include/pic24_all.h"
#include "../../include/esos_f15_ui.h"
#include "../../include/can_config.h"
#include <stdio.h>


ESOS_USER_TASK(CAN_SNIFFER)  {
  ESOS_TASK_BEGIN();
  static uint8_t u8_rx_buff_id; // create this for holding the rx buffer id
  while (TRUE) {
    _DMA0IF = 1;
    DELAY_MS(10);  //delay for reception
    if (_DMA1IF) {
      _DMA1IF = 0;   //RX message accepted
      u8_rx_buff_id = GET_FIFO_READBUFFER_ECAN1();
      printf("BUFFER ID = %i\n\r", u8_rx_buff_id); // print the current buffer id
      printf("SID ID = %02x\n\r", msgBuf[u8_rx_buff_id].w0.SID); // the message ID
      // print out the data
      printf("MSG 0x= %i ", msgBuf[u8_rx_buff_id].data.u8[7]);
      printf("%02x ", msgBuf[u8_rx_buff_id].data.u8[6]);
      printf("%02x ", msgBuf[u8_rx_buff_id].data.u8[5]);
      printf("%02x ", msgBuf[u8_rx_buff_id].data.u8[4]);
      printf("%02x ", msgBuf[u8_rx_buff_id].data.u8[3]);
      printf("%02x ", msgBuf[u8_rx_buff_id].data.u8[2]);
      printf("%02x ", msgBuf[u8_rx_buff_id].data.u8[1]);
      printf("%02x\n\r", msgBuf[u8_rx_buff_id].data.u8[0]);
      clrRxFullFlagECAN1(u8_rx_buff_id);
    }
  } // endof while(TRUE)
  ESOS_TASK_END();
} // END OF DAC_MENU TASK

void user_init(void) {
  configECAN1();
  CHANGE_MODE_ECAN1(ECAN_MODE_NORMAL); // DO NOT FILTER ECAN_LISTEN_ALL_MESSAGES
  esos_RegisterTask(CAN_SNIFFER); 
} // end user_init()