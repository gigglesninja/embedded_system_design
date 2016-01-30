// TEAM APPLE!

// FINAL TASK!!! THANK GOD THIS IS OVER!!!!

// INCLUDEs go here  (First include the main esos.h file)
//      After that, the user can include what they need
//#include "revF15.h"
#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24_spi.h"
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24_i2c.h"
#include "../../include/esos_f15_ui.h"
#include <stdio.h>
#include "stdbool.h"


#define DS1631ADDR 0x90   //DS1631 address with all pins tied low
#define ACCESS_CONFIG 0xAC
#define CONFIG_COMMAND 0x0C //continuous conversion, 12-bit mode
#define START_CONVERT 0x51
#define READ_TEMP 0xAA

ESOS_USER_TASK(I2C_CONTROLLER1) {
  static uint8_t u8_hi, u8_lo;
  ESOS_TASK_BEGIN();
  while (TRUE) {
    // store incoming options until P is written
      // signal send
      ESOS_TASK_WAIT_TICKS(30000);
      ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
      // write the info from the buffer
      ESOS_TASK_WAIT_ON_WRITE1I2C1(DS1631ADDR, START_CONVERT);
      ESOS_TASK_WAIT_ON_WRITE1I2C1(DS1631ADDR, READ_TEMP);
      ESOS_TASK_WAIT_ON_READ2I2C1(DS1631ADDR, u8_hi, u8_lo);
      //ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
      //ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_hi);
      //ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_lo);
      //ESOS_TASK_WAIT_ON_SEND_STRING("\r\n");
      //ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
      ESOS_TASK_SIGNAL_AVAILABLE_I2C();
      // wait before this task can run again
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end upper_case()

void user_init(void) {
  config_esos_f15ui();
  esos_pic24_configI2C1(400);
  esos_RegisterTask(I2C_CONTROLLER1);
}
