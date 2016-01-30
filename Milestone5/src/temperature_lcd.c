// TEAM APPLE!

// .. "Copyright (c) 2008 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
//    All rights reserved.
//    (R. Reese, reese_AT_ece.msstate.edu, Mississippi State University)
//    (B. A. Jones, bjones_AT_ece.msstate.edu, Mississippi State University)
//    (J. W. Bruce, jwbruce_AT_ece.msstate.edu, Mississippi State University)
//
//    Permission to use, copy, modify, and distribute this software and its
//    documentation for any purpose, without fee, and without written agreement is
//    hereby granted, provided that the above copyright notice, the following
//    two paragraphs and the authors appear in all copies of this software.
//
//    IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
//    DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
//    OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
//    HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//    THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
//    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//    AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
//    ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
//    PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
//
//    Please maintain this header in its entirety when copying/modifying
//    these files.
//
// *********************************
// esos_skel.c - basic ESOS template
// *********************************
// ESOS application program similar to the "Echo" program echo.c
// in Chapter 8.  (See Figure 8.6 in the text.)
// This version of "echo" program which waits for UART1 RX character
// and echos it back to UART1 TX with all lowercase letters converted
// to uppercase.
// Use this program to test your UART connection under ESOS.
//
// Application also has a flashing LED on RB15.  Flashing LED is generated
// by a *user task* that runs simultaneously with the echo task above.
//
//  .. note::
//
//     Demonstrates multitasking in ESOS



// INCLUDEs go here  (First include the main esos.h file)
//      After that, the user can include what they need
//#include "revF15.h"
#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../include/esos_f15_ui.h"
#include "../../include/esos_pic24_sensor.h"
#include "../../include/esos_pic24_lcd.h"


// GLOBALs go here
#define LM60_OFFSET             0xA5A0   // 424 * 100
#define LM60_CELSIUS_CONSTANT   0x0271   // 6.25 *100

bool b_showPOT;

//  Generally, the user-created semaphores will be defined/allocated here

ESOS_SEMAPHORE( sem_cusCharSet );

/************************************************************************
 * User supplied functions
 ************************************************************************
 */

// This turns the hex value into a char for the ASCI table
char u8_hexToChar(uint8_t u8_char) {
  if(u8_char == 0x0) {
    return '0';
  }
  else if(u8_char == 0x1) {
    return '1';
  }
  else if(u8_char == 0x2) {
    return '2';
  }
  else if(u8_char == 0x3) {
    return '3';
  }
  else if(u8_char == 0x4) {
    return '4';
  }
  else if(u8_char == 0x5) {
    return '5';
  }
  else if(u8_char == 0x6) {
    return '6';
  }
  else if(u8_char == 0x7) {
    return '7';
  }
  else if(u8_char == 0x8) {
    return '8';
  }
  else if(u8_char == 0x9) {
    return '9';
  }
  else if(u8_char == 0xa) {
    return 'A';
  }
  else if(u8_char == 0xb) {
    return 'B';
  }
  else if(u8_char == 0xc) {
    return 'C';
  }
  else if(u8_char == 0xd) {
    return 'D';
  }
  else if(u8_char == 0xe) {
    return 'E';
  }
  return 'F';
}

// 20 to 35 C Hex range is 15-23
// There might be an easier way to do this. Since the specs only
// say the sensor should display 20C to 35C this makes for an easy way
uint8_t u8_hexToTemp(uint8_t u8_upper, uint8_t u8_lower) {
  uint8_t u8_data;
  u8_upper = u8_upper * 16;
  u8_data = u8_upper + u8_lower;
  if(u8_data <= 20) {
    return 0x20;
  }
  else if(u8_data == 0x15) {
    return 0x21;
  }
  else if(u8_data == 0x16) {
    return 0x22;
  }
  else if(u8_data == 0x17) {
    return 0x23;
  }
  else if(u8_data == 0x18) {
    return 0x24;
  }
  else if(u8_data == 0x19) {
    return 0x25;
  }
  else if(u8_data == 0x1a) {
    return 0x26;
  }
  else if(u8_data == 0x1b) {
    return 0x27;
  }
  else if(u8_data == 0x1c) {
    return 0x28;
  }
  else if(u8_data == 0x1d) {
    return 0x29;
  }
  else if(u8_data == 0x1e) {
    return 0x30;
  }
  else if(u8_data == 0x1f) {
    return 0x31;
  }
  else if(u8_data == 0x20) {
    return 0x32;
  }
  else if(u8_data == 0x21) {
    return 0x33;
  }
  else if(u8_data == 0x22) {
    return 0x34;
  }
  return 0x35;
}

// ESOS TASK

ESOS_USER_TASK(CHECK_SW3)  {
  ESOS_TASK_BEGIN();
  static uint8_t pu8_customChar0[8] = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};
  static uint8_t pu8_customChar1[8] = {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07};


  static uint8_t pu8_customChar2[8] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x1F, 0x1F};
  static uint8_t pu8_customChar3[8] = {0x20, 0x20, 0x20, 0x20, 0x1F, 0x1F, 0x1F, 0x1F};
  static uint8_t pu8_customChar4[8] = {0x20, 0x20, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};
  esos_lcd_setCustomChar(0x40, pu8_customChar0);
  esos_lcd_setCustomChar(0x48, pu8_customChar1);
  esos_lcd_setCustomChar(0x50, pu8_customChar4);
  esos_lcd_setCustomChar(0x58, pu8_customChar3);
  esos_lcd_setCustomChar(0x60, pu8_customChar2);
  while (TRUE) { 
    ESOS_TASK_F15UI_WAIT_UNTIL_SW3_PRESSED(); // block this task until we need it
    b_showPOT = !b_showPOT;
    esos_lcd_clearScreen();
    ESOS_TASK_F15UI_WAIT_UNTIL_SW3_RELEASED();
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end upper_case()

ESOS_USER_TASK(POT) {
  ESOS_TASK_BEGIN();
  static bool b_constants = FALSE;
  static uint16_t u16_data;
  static uint8_t u8_upper;
  static uint8_t u8_middle;
  static uint8_t u8_lower;
  static uint8_t u8_CursPosition;
  static uint8_t u8_CursSubDiv;
  static uint8_t u8_newCursSubDiv;
  static uint8_t u8_newCursPosition;
  while (TRUE) {
    if(b_showPOT) {
      // configure the ADC
      ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(ESOS_SENSOR_CH02, ESOS_SENSOR_VREF_3V3);
      ESOS_USE_ADC_12_BIT(TRUE);
      if(!b_constants) { // if the screen constants are not written write them
        // this could be a string _________
        esos_lcd_setCursor(ROW_ONE, 1 );
        ESOS_TASK_WAIT_LCD_WRITE_DATA('P');
        ESOS_TASK_WAIT_LCD_WRITE_DATA('O');
        ESOS_TASK_WAIT_LCD_WRITE_DATA('T');
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
        ESOS_TASK_WAIT_LCD_WRITE_DATA('x');
        ESOS_TASK_WAIT_LCD_WRITE_DATA('0');
        ESOS_TASK_WAIT_LCD_WRITE_DATA('0');
        ESOS_TASK_WAIT_LCD_WRITE_DATA('0');
      b_constants = TRUE;
        //_________________________________
      }
      ESOS_TASK_WAIT_SENSOR_READ(u16_data, ESOS_SENSOR_AVG64,
                                 ESOS_SENSOR_FORMAT_BITS); // get the ADC reading
      ESOS_TASK_WAIT_UNTIL(ESOS_SENSOR_CLOSE());
      u8_upper = u16_data >> 8;
      u8_middle = (uint8_t)u16_data & 0xF0;
      u8_middle = u8_middle >> 4;
      u8_lower = (uint8_t)u16_data & 0x0F;
      esos_lcd_setCursor(ROW_ONE, 6 ); // move cursor to upper byte position
      ESOS_TASK_WAIT_LCD_WRITE_DATA(u8_hexToChar(u8_upper)); // write upper byte
      esos_lcd_setCursor(ROW_ONE, 7 ); // move cursor to middle byte position
      ESOS_TASK_WAIT_LCD_WRITE_DATA(u8_hexToChar(u8_middle)); // write middle byte
      esos_lcd_setCursor(ROW_ONE, 8 ); // move cursor to lower byte position
      ESOS_TASK_WAIT_LCD_WRITE_DATA(u8_hexToChar(u8_lower)); // write lower byte
      //--------------- ADD SLIDER CODE HERE----------------------------------------
       u8_CursPosition = (1+u16_data)/512;
      if(u8_CursPosition != 8){
        u8_CursPosition++;
      }
      u8_CursSubDiv = (u16_data-((u8_CursPosition-1)*512))/256;
      if(u8_CursSubDiv == 3){
        u8_CursSubDiv = 2;
      }
      if((u8_newCursSubDiv !=  u8_CursSubDiv) || (u8_newCursPosition != u8_CursPosition)){
        esos_lcd_setCursor(ROW_TWO, 1);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
        esos_lcd_setCursor(ROW_TWO, 2);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
        esos_lcd_setCursor(ROW_TWO, 3);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
        esos_lcd_setCursor(ROW_TWO, 4);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
        esos_lcd_setCursor(ROW_TWO, 5);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
        esos_lcd_setCursor(ROW_TWO, 6);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
        esos_lcd_setCursor(ROW_TWO, 7);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
        esos_lcd_setCursor(ROW_TWO, 8);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
        u8_newCursSubDiv = u8_CursSubDiv;
        u8_newCursPosition = u8_CursPosition;
      }
      esos_lcd_setCursor(ROW_TWO, u8_CursPosition);
      ESOS_TASK_WAIT_LCD_WRITE_DATA(u8_CursSubDiv);
      //----------------------------------------------------------------------------
    }
    else {
      b_constants = FALSE;
    }
    ESOS_TASK_WAIT_TICKS(15);
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end upper_case()

ESOS_USER_TASK(TEMP) {
  ESOS_TASK_BEGIN();
  static bool b_constants = FALSE;
  static uint16_t u16_data;
  static uint8_t u8_temp;
  static uint8_t u8_oldtemp;
  static uint8_t u8_upper;
  static uint8_t u8_lower;
  while (TRUE) {
    if(!b_showPOT) {
      // configure the ADC
      ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(ESOS_SENSOR_CH03, ESOS_SENSOR_VREF_3V3);
      ESOS_USE_ADC_12_BIT(TRUE);
      if(!b_constants) { // if the screen constants are not written write them
        // this could be a string _________
        esos_lcd_setCursor(ROW_ONE, 1);
        ESOS_TASK_WAIT_LCD_WRITE_DATA('L');
        ESOS_TASK_WAIT_LCD_WRITE_DATA('M');
        ESOS_TASK_WAIT_LCD_WRITE_DATA('6');
        ESOS_TASK_WAIT_LCD_WRITE_DATA('0');
        esos_lcd_setCursor(ROW_TWO, 3);
        ESOS_TASK_WAIT_LCD_WRITE_DATA('C');
      	b_constants = TRUE;
        //_________________________________
      }
      ESOS_TASK_WAIT_SENSOR_READ(u16_data, ESOS_SENSOR_AVG64, ESOS_SENSOR_FORMAT_VOLTAGE); // get the ADC reading
      ESOS_TASK_WAIT_UNTIL(ESOS_SENSOR_CLOSE());
      // temp calculation is Vo = (6.25mv/C * Tc) + 424mV
      u16_data = (u16_data * 10) - LM60_OFFSET;
      u16_data = u16_data / LM60_CELSIUS_CONSTANT;
      // first we need to separate the powers
      u8_upper = (uint8_t)u16_data & 0xF0;
      u8_upper = u8_upper >> 4;
      u8_lower = (uint8_t)u16_data & 0x0F;
      // next convert it to a hex decimal representation
      u8_temp = u8_hexToTemp(u8_upper,u8_lower);
      u8_upper = (uint8_t)u8_temp & 0xF0;
      u8_upper = u8_upper >> 4;
      u8_lower = (uint8_t)u8_temp & 0x0F;
      esos_lcd_setCursor(ROW_TWO, 1 ); // move cursor to upper byte position
      ESOS_TASK_WAIT_LCD_WRITE_DATA(u8_hexToChar(u8_upper)); // write upper byte
      esos_lcd_setCursor(ROW_TWO, 2 ); // move cursor to upper byte position
      ESOS_TASK_WAIT_LCD_WRITE_DATA(u8_hexToChar(u8_lower)); // write upper byte

      //--------------- ADD SLIDER CODE HERE----------------------------------------

      /******************************************************************************
    20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 | 35
                                                                               __ 15
                                                                      __   __ 14
                                                                     __   __   __ 13
                                                                __   __   __   __ 12
                                                           __   __   __   __   __ 11
                                                      __   __   __   __   __   __ 10
                                                 __   __   __   __   __   __   __ 9
                                            __   __   __   __   __   __   __   __ 8
                                       __   __   __   __   __   __   __   __   __ 7
                                  __   __   __   __   __   __   __   __   __   __ 6
                             __   __   __   __   __   __   __   __   __   __   __ 5
                        __   __   __   __   __   __   __   __   __   __   __   __ 4
                   __   __   __   __   __   __   __   __   __   __   __   __   __ 3
              __   __   __   __   __   __   __   __   __   __   __   __   __   __ 2
         __   __   __   __   __   __   __   __   __   __   __   __   __   __   __ 1
    __   __   __   __   __   __   __   __   __   __   __   __   __   __   __   __ 0 

      ******************************************************************************/

      if(u8_oldtemp != u8_temp){
        // the temp is in a hex representation of C
        // so need to compare the hex value
        // 27C == 0x27 (this is for LCD printing purpose)
        if(u8_temp >= 0x26){
          // if >= 27 row two will be full
          esos_lcd_setCursor(ROW_TWO, 8 );
          ESOS_TASK_WAIT_LCD_WRITE_DATA(0xFF);
          esos_lcd_setCursor(ROW_ONE, 8 );
          ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
          esos_lcd_setCursor(ROW_ONE, 8 );
          // check to see what needs to be put into row one
          if(u8_temp >= 0x34){
            ESOS_TASK_WAIT_LCD_WRITE_DATA(0xFF);
          }
          else if(u8_temp >= 0x32){
            ESOS_TASK_WAIT_LCD_WRITE_DATA(0x02);
          }
          else if(u8_temp >= 0x30){
            ESOS_TASK_WAIT_LCD_WRITE_DATA(0x03);
          }
          else if(u8_temp >= 0x28){
            ESOS_TASK_WAIT_LCD_WRITE_DATA(0x04);
          }
        }
        else{
          // if < 27 row one will be blank
          esos_lcd_setCursor(ROW_ONE, 8 );
          ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
          esos_lcd_setCursor(ROW_TWO, 8 );
          // check to see what needs to be put into row two
          if(u8_temp <= 0x21){
            ESOS_TASK_WAIT_LCD_WRITE_DATA(0x04);
          }
          else if(u8_temp <= 0x23){
            ESOS_TASK_WAIT_LCD_WRITE_DATA(0x03);
          }
          else if(u8_temp <= 0x25){
            ESOS_TASK_WAIT_LCD_WRITE_DATA(0x02);
          }
        }
        u8_oldtemp = u8_temp; // reset old temp
      }
     
      //----------------------------------------------------------------------------
    }
    else {
      u8_oldtemp = 0; //
      b_constants = FALSE;
    }
    ESOS_TASK_WAIT_TICKS(100);
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end upper_case()

/****************************************************
 *  user_init()
 ****************************************************
 */
/*-------------------------------------------------------------
There are 8 character slots, so each slot represents a data sample of 128. In other words,
the sensor needs to change by 128 units to move to the next character block.
However, I have created custom characters that are simply "|" but there is one for the 5 different
positions it could be in per character block. For exmample:
Custom char 0 is: | * * * *     Custom char 1 is: * | * * *   The subsequent custom chars follow the same pattern (custom char 4 is not working correctly..)
                  | * * * *                       * | * * *
                  | * * * *                       * | * * *
                  | * * * *                       * | * * *
                  | * * * *                       * | * * *
                  | * * * *                       * | * * *
                  | * * * *                       * | * * *
                  | * * * *                       * | * * *
Each of these lines represent movement of 25 units (truncated from 25.6). This value is stored in u8_CursSubDiv, and there are 5 different possibilities
*/
void user_init(void) {
  config_esos_f15ui();    // setup UI for heartbeat
  f15ui_flashLED3(500);
  esos_lcd_configDisplay(); // config LCD hardware
  esos_lcd_init();      // initilize the LCD
  b_showPOT = TRUE;
  CONFIG_TEMP();      // setup the tempature sensor
  // user_init() should register at least one user task
  esos_RegisterTask(POT);
  esos_RegisterTask(TEMP);
  esos_RegisterTask(CHECK_SW3);
} // end user_init()
