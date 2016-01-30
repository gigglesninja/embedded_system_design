// TEAM APPLE!

/*** I N C L U D E S **********************************************************/
#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../../pic24lib_all/lib/include/pic24_adc.h"
#include "../../include/esos_f15_ui.h"
#include "../../include/esos_pic24_sensor.h"

/*** G L O B A L S ************************************************************/
uint8_t u8_ProcMask = ESOS_SENSOR_ONE_SHOT; // initial setup for one shot
bool b_InSW2Loop; 
bool b_InSW3Loop = false;
#define SENSOR_CHANNEL ESOS_SENSOR_CH03 // use CH2 on this task

/*** U S E R   T A S K S ******************************************************/
// task will handle SW1
ESOS_USER_TASK(READ_POT_SW1) {
  ESOS_TASK_BEGIN();
  static uint16_t u16_data;
  static bool b_SW1Loop = FALSE;
  while(true) {
    ESOS_TASK_WAIT_UNTIL(f15ui_isSW1Pressed()); // wait for SW1
    b_SW1Loop = TRUE; // this could be removed
    while(b_SW1Loop) { // this could be removed
      b_SW1Loop = FALSE; // this could be removed
      if(!b_InSW3Loop){
            ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(SENSOR_CHANNEL, ESOS_SENSOR_VREF_3V3); // wait for sensor setup
            ESOS_USE_ADC_12_BIT(TRUE); // use 12 bits
            ESOS_TASK_WAIT_SENSOR_READ(u16_data, u8_ProcMask, ESOS_SENSOR_FORMAT_BITS); // read the sensor
            ESOS_TASK_WAIT_UNTIL(ESOS_SENSOR_CLOSE()); // close the sensor
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM(); 
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t)u16_data);
            ESOS_TASK_WAIT_ON_SEND_STRING("\n\r");
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
      }
      ESOS_TASK_WAIT_TICKS(10);
    }
    ESOS_TASK_WAIT_UNTIL(f15ui_isSW1Released()); // wait for SW2
  }
  ESOS_TASK_END();
}

// task will handle SW2
ESOS_USER_TASK(READ_POT_SW2) {
  ESOS_TASK_BEGIN();
  static uint16_t u16_data;
  static bool b_SW2Loop = FALSE;
  b_InSW2Loop = FALSE;
  while(true) {
    ESOS_TASK_WAIT_UNTIL(f15ui_isSW2Pressed()); // wait for SW2 press
    b_SW2Loop = TRUE;
    b_InSW2Loop = TRUE;
      while(b_SW2Loop) {
        if(!b_InSW3Loop){
          ESOS_USE_ADC_12_BIT(TRUE); // use 12 bits
          ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(SENSOR_CHANNEL, ESOS_SENSOR_VREF_3V3); // wait for sensor setup
          ESOS_TASK_WAIT_SENSOR_READ(u16_data, u8_ProcMask, ESOS_SENSOR_FORMAT_BITS); // read process and format sensor
          ESOS_TASK_WAIT_UNTIL(ESOS_SENSOR_CLOSE()); // close and turn off the sensor
          ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
          ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t)u16_data);
          ESOS_TASK_WAIT_ON_SEND_STRING("\n\r");
          ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
          //The purpose of the following block of code is to allow the user to more easily break out of the loop started by SW2
          ESOS_TASK_WAIT_TICKS(250);
          //The user is given 0.5 second (see line above) to release the button, otherwise the task will exit immediately
          if(f15ui_isSW1Pressed() || f15ui_isSW2Pressed()) {
            b_InSW2Loop = FALSE;
            b_SW2Loop = FALSE;
          }
          ESOS_TASK_WAIT_TICKS(250);
          //The user is given 0.5 second (see line above) to release the button, otherwise the task will exit immediately
          if(f15ui_isSW1Pressed() || f15ui_isSW2Pressed()) {
            b_InSW2Loop = FALSE;
            b_SW2Loop = FALSE;
          }
          ESOS_TASK_WAIT_TICKS(250);
          //The user is given 0.5 second (see line above) to release the button, otherwise the task will exit immediately
          if(f15ui_isSW1Pressed() || f15ui_isSW2Pressed()) {
            b_InSW2Loop = FALSE;
            b_SW2Loop = FALSE;
          }
          ESOS_TASK_WAIT_TICKS(240);
          //The user is given 0.5 second (see line above) to release the button, otherwise the task will exit immediately
          if(f15ui_isSW1Pressed() || f15ui_isSW2Pressed()) {
            b_InSW2Loop = FALSE;
            b_SW2Loop = FALSE;
          }
      }
      ESOS_TASK_WAIT_TICKS(10);
    }
    ESOS_TASK_WAIT_UNTIL(f15ui_isSW2Released()); // wait for SW2 release
  }
  ESOS_TASK_END();
}

// this task will print out the menu
ESOS_USER_TASK(READ_POT_SW3) {
  ESOS_TASK_BEGIN();
  static uint8_t u8_choice;
  static uint8_t u8_SampleNumber;
  while(true) {
    ESOS_TASK_WAIT_UNTIL(f15ui_isSW3Pressed()); // wait for SW3 press
    b_InSW3Loop = true;
    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("Choose from the following processing modes: \n\r");
    ESOS_TASK_WAIT_ON_SEND_STRING("1: \"One-shot\"\n\r");
    ESOS_TASK_WAIT_ON_SEND_STRING("2: \"Average\"\n\r");
    ESOS_TASK_WAIT_ON_SEND_STRING("3: \"Minimum\"\n\r");
    ESOS_TASK_WAIT_ON_SEND_STRING("4: \"Maximum\"\n\r");
    ESOS_TASK_WAIT_ON_SEND_STRING("5: \"Median\"\n\r");
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    ESOS_TASK_WAIT_ON_GET_UINT8(u8_choice);
    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
    // set up the process mask based on user choice
    if(u8_choice == '1') {
      u8_ProcMask = ESOS_SENSOR_ONE_SHOT;
    }
    else {
      ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
      ESOS_TASK_WAIT_ON_SEND_STRING("Type the number of samples to be used.\n\r");
      ESOS_TASK_WAIT_ON_SEND_STRING("Valid choices are: \n\r");
      ESOS_TASK_WAIT_ON_SEND_STRING("1: 2\n\r");
      ESOS_TASK_WAIT_ON_SEND_STRING("2: 4\n\r");
      ESOS_TASK_WAIT_ON_SEND_STRING("3: 8\n\r");
      ESOS_TASK_WAIT_ON_SEND_STRING("4: 16\n\r");
      ESOS_TASK_WAIT_ON_SEND_STRING("5: 32\n\r");
      ESOS_TASK_WAIT_ON_SEND_STRING("6: 64\n\r");
      ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
      ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
      ESOS_TASK_WAIT_ON_GET_UINT8(u8_SampleNumber);
      ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
      if(u8_choice == '2') {
        if(u8_SampleNumber == '1') {
          u8_ProcMask = ESOS_SENSOR_AVG2;
        }
        else if(u8_SampleNumber == '2') {
          u8_ProcMask = ESOS_SENSOR_AVG4;
        }
        else if(u8_SampleNumber == '3') {
          u8_ProcMask = ESOS_SENSOR_AVG8;
        }
        else if(u8_SampleNumber == '4') {
          u8_ProcMask = ESOS_SENSOR_AVG16;
        }
        else if(u8_SampleNumber == '5') {
          u8_ProcMask = ESOS_SENSOR_AVG32;
        }
        else {
          u8_ProcMask = ESOS_SENSOR_AVG64;
        }
      }
      else if(u8_choice == '3') {
        if(u8_SampleNumber == '1') {
          u8_ProcMask = ESOS_SENSOR_MIN2;
        }
        else if(u8_SampleNumber == '2') {
          u8_ProcMask = ESOS_SENSOR_MIN4;
        }
        else if(u8_SampleNumber == '3') {
          u8_ProcMask = ESOS_SENSOR_MIN8;
        }
        else if(u8_SampleNumber == '4') {
          u8_ProcMask = ESOS_SENSOR_MIN16;
        }
        else if(u8_SampleNumber == '5') {
          u8_ProcMask = ESOS_SENSOR_MIN32;
        }
        else {
          u8_ProcMask = ESOS_SENSOR_MIN64;
        }
      }
      else if(u8_choice == '4') {
        if(u8_SampleNumber == '1') {
          u8_ProcMask = ESOS_SENSOR_MAX2;
        }
        else if(u8_SampleNumber == '2') {
          u8_ProcMask = ESOS_SENSOR_MAX4;
        }
        else if(u8_SampleNumber == '3') {
          u8_ProcMask = ESOS_SENSOR_MAX8;
        }
        else if(u8_SampleNumber == '4') {
          u8_ProcMask = ESOS_SENSOR_MAX16;
        }
        else if(u8_SampleNumber == '5') {
          u8_ProcMask = ESOS_SENSOR_MAX32;
        }
        else {
          u8_ProcMask = ESOS_SENSOR_MAX64;
        }
      }
      else {
        if(u8_SampleNumber == '1') {
          u8_ProcMask = ESOS_SENSOR_MEDIAN2;
        }
        else if(u8_SampleNumber == '2') {
          u8_ProcMask = ESOS_SENSOR_MEDIAN4;
        }
        else if(u8_SampleNumber == '3') {
          u8_ProcMask = ESOS_SENSOR_MEDIAN8;
        }
        else if(u8_SampleNumber == '4') {
          u8_ProcMask = ESOS_SENSOR_MEDIAN16;
        }
        else if(u8_SampleNumber == '5') {
          u8_ProcMask = ESOS_SENSOR_MEDIAN32;
        }
        else {
          u8_ProcMask = ESOS_SENSOR_MEDIAN64;
        }
      }
    }
    b_InSW3Loop = false;
    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("SAMPLE SET\n\r"); // let user know the sample has been set
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_UNTIL(f15ui_isSW3Released()); // wait for SW3 release so we don't loop
  }
  ESOS_TASK_END();
}

void user_init(void) {
  CONFIG_POT();
  config_esos_f15ui();
  f15ui_flashLED3(250);
  esos_RegisterTask(READ_POT_SW1);
  esos_RegisterTask(READ_POT_SW2);
  esos_RegisterTask(READ_POT_SW3);
}