// TEAM APPLE!

/*** I N C L U D E S **********************************************************/
#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../../pic24lib_all/lib/include/pic24_adc.h"
#include "../../include/esos_f15_ui.h"
#include "../../include/esos_pic24_sensor.h"

/*** U S E R   T A S K S ******************************************************/
ESOS_USER_TASK(READ_POT_SW1) {
  ESOS_TASK_BEGIN();
  static uint16_t u16_data;
  while(true) {
    // wait for SW1 pressed
    ESOS_TASK_F15UI_WAIT_UNTIL_SW1_PRESSED();
    ESOS_USE_ADC_12_BIT(TRUE); // since we use 1 channel use 12 bits
    ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(ESOS_SENSOR_CH02, ESOS_SENSOR_VREF_3V3); // wait and setup sensor
    ESOS_TASK_WAIT_SENSOR_QUICK_READ(u16_data); // get sensor data
    ESOS_TASK_WAIT_UNTIL(ESOS_SENSOR_CLOSE()); // close the sensor
    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM(); // wait for communictions output
    ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t)u16_data); // print sensor data
    ESOS_TASK_WAIT_ON_SEND_STRING("\n\r"); 
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    ESOS_TASK_F15UI_WAIT_UNTIL_SW1_RELEASED(); // wait for SW1 released so we don't go back into the loop
  }
  ESOS_TASK_END();
}

ESOS_USER_TASK(READ_POT_SW2) {
  ESOS_TASK_BEGIN();
  static uint16_t u16_data;
  static bool b_loop;
  while(true) {
    ESOS_TASK_F15UI_WAIT_UNTIL_SW2_PRESSED(); //wait for SW2 press
    b_loop = TRUE; 
    while(b_loop) { // print out the data every 1 second
      ESOS_USE_ADC_12_BIT(TRUE); // since we use 1 channel use 12 bits
      ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(ESOS_SENSOR_CH02, ESOS_SENSOR_VREF_3V3);// wait and setup sensor
      ESOS_TASK_WAIT_SENSOR_QUICK_READ(u16_data); // get sensor data
      ESOS_TASK_WAIT_UNTIL(ESOS_SENSOR_CLOSE());
      ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
      ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t)u16_data);
      ESOS_TASK_WAIT_ON_SEND_STRING("\n\r");
      ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
      ESOS_TASK_WAIT_TICKS(1000);
      // if SW1 or SW2 is pressed break out of the loop
      if(f15ui_isSW1Pressed() || f15ui_isSW2Pressed()) {
        b_loop = false;
      }
    }
    ESOS_TASK_F15UI_WAIT_UNTIL_SW2_RELEASED(); // wait until SW2 is released so we don't loop
  }
  ESOS_TASK_END();
}

/*** U S E R   I N I T ********************************************************/
void user_init(void) {
  CONFIG_POT(); // setup or POT hardware
  config_esos_f15ui(); // setup our user interface
  f15ui_flashLED3(250); // 500ms HB
  esos_RegisterTask(READ_POT_SW1); // register task
  esos_RegisterTask(READ_POT_SW2); // register task
}