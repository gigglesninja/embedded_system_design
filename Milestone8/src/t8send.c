#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../../pic24lib_all/esos/include/esos_mail.h"
#include "../../include/esos_can.h"
#include "../../include/revF15.h"
#include "../../include/esos_task_CAN.h"
#include "../../include/esos_f15_ui.h"
#include "../../include/can_config.h"
#include <stdio.h>


ESOS_USER_TASK(CAN_FIFO)  {
  //REGISTER_TO_CAN(CAN_FIFO,123);
  static MAILMESSAGE st_Msg;
  static ESOS_TASK_HANDLE CAN_Factory;
  ESOS_TASK_BEGIN();
  CAN_Factory = esos_GetTaskHandle(CANFactory);
  while (TRUE) {

    if (SW1_PRESSED()) {
    LED1 = 1;
    } else if (SW1_RELEASED()) {
        LED1 = 0;   
    }
    
    if(SW2_PRESSED()) {
        LED2 = 1;
    } else if (SW2_RELEASED()) {
        LED2 = 0;
    }
    //printf("RUNNING? ");
    ESOS_TASK_MAKE_MSG_CAN_2_BYTE(st_Msg, 123, LED1, LED2);
    ESOS_TASK_WAIT_ON_TASKS_MAILBOX_HAS_AT_LEAST(CAN_Factory,  __MAIL_MSG_HEADER_LEN + sizeof(uint16_t));
    ESOS_TASK_SEND_MESSAGE(CAN_Factory, (&st_Msg));
    //printf("YES.\n\r");
    ESOS_TASK_WAIT_TICKS(2000);

  } // endof while(TRUE)
  ESOS_TASK_END();
} // END OF DAC_MENU TASK

void user_init(void) {

  config_esos_f15ui();
  f15ui_flashLED3(1000);
  configECAN1();
  CHANGE_MODE_ECAN1(ECAN_MODE_NORMAL);
  esos_RegisterTask(CANFactory);
  esos_RegisterTask(CAN_FIFO);
} // end user_init()