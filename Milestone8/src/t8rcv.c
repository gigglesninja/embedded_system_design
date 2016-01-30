#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24_spi.h"
#include "../../../pic24lib_all/lib/include/pic24_all.h"
#include "../../include/esos_f15_ui.h"
#include "../../include/can_config.h"
#include <stdio.h>

ESOS_USER_TASK(CAN_FIFO)  {
  ESOS_TASK_BEGIN();
  static uint32_t u32_in0, u32_in1;
  static uint8_t rx_buff_id;
  static uint16_t u16_msg_ID;
  while (TRUE) {
    ESOS_TASK_WAIT_UNTIL(_DMA1IF) ;
      _DMA1IF = 0;   //RX message accepted
      rx_buff_id = GET_FIFO_READBUFFER_ECAN1();
      u32_in0 = msgBuf[rx_buff_id].data.u32[0];
      u32_in1 = msgBuf[rx_buff_id].data.u32[1];
      u16_msg_ID = msgBuf[rx_buff_id].w0.SID;

      uint8_t u8_led1 = (u32_in0 & 0x00000001)? 1:0; // Just get LSb of Byte 1;
      uint8_t u8_led2 = (u32_in0 & 0x00000100)? 1:0; // Just get LSb of Byte 2;
      LED1 = u8_led1;
      LED2 = u8_led2;
      printf("LED1 State = %d, LED2 State = %d\n\r", u8_led1, u8_led2 );

      clrRxFullFlagECAN1(rx_buff_id);
    
  } // endof while(TRUE)
  ESOS_TASK_END();
} // END OF DAC_MENU TASK

void user_init(void) {
  config_esos_f15ui();
  f15ui_flashLED3(1000);
  configECAN1();
  CHANGE_MODE_ECAN1(ECAN_MODE_NORMAL);  //loopback to ourself for a test.
  esos_RegisterTask(CAN_FIFO); // for change in wave 
} // end user_init()