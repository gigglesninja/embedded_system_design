#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24_spi.h"
#include "../../../pic24lib_all/lib/include/pic24_all.h"
#include "../../include/esos_lcd_menu.h"
#include "../../include/can_config.h"
#include <stdio.h>

ESOS_USER_TASK(CAN_FIFO)  {
  ESOS_TASK_BEGIN();
  static uint32_t u32_out0, u32_out1, u32_in0, u32_in1;
  static uint8_t rx_buff_id, u8_cnt;
  u32_out0 = 0xFEDCBA98;
  u32_out1 = 0x76543210;
  u8_cnt = 0;
  IEC0bits.DMA0IE = 1;
  while (TRUE) {
    msgBuf[0].data.u32[0] = u32_out0; //save in CAN message
    msgBuf[0].data.u32[1] = u32_out1;
    //format Buffer 0 for TX with SID=MSG_ID, data length = 8 bytes
    formatStandardDataFrameECAN(&msgBuf[0], MSG_ID+u8_cnt, 8);
    startTxECAN1(0);  //start transmission of buffer 0
    ESOS_TASK_WAIT_WHILE(getTxInProgressECAN1(0));
    _DMA0IF = 1;
    ESOS_TASK_WAIT_TICKS(10);  //delay for reception
    if (!_DMA1IF) {
      printf("Message ID 0x%X rejected by acceptance filter.\n\r",MSG_ID+u8_cnt);
    } else {
      _DMA1IF = 0;   //RX message accepted
      rx_buff_id = GET_FIFO_READBUFFER_ECAN1();
      u32_in0 = msgBuf[rx_buff_id].data.u32[0];
      u32_in1 = msgBuf[rx_buff_id].data.u32[1];
      printf("Rx Buff: %d. Msg ID: 0x%X, Out: 0x%08lX%08lX, In: 0x%08lX%08lX\n\r",
             rx_buff_id, msgBuf[rx_buff_id].w0.SID,u32_out0, u32_out1, u32_in0, u32_in1 );
      clrRxFullFlagECAN1(rx_buff_id);
    }
    u32_out0 = rrot32(u32_out0);
    u32_out1 = rrot32(u32_out1);
    u8_cnt++;
    if (u8_cnt == 8){
      u8_cnt = 0;
    }
    LED3 = !LED3;
    ESOS_TASK_WAIT_TICKS(500);
  } // endof while(TRUE)
  ESOS_TASK_END();
} // END OF DAC_MENU TASK

void user_init(void) {
  configECAN1();
  CHANGE_MODE_ECAN1(ECAN_MODE_LOOPBACK);  //loopback to ourself for a test.
  esos_RegisterTask(CAN_FIFO); // for change in wave 
} // end user_init()