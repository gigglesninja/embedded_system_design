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
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24_spi.h"
#include "../../include/esos_lcd_menu.h"

/************************************************************************
 * User supplied functions
 ************************************************************************
 */

// **************** CONFIG THE DAC **************************************
#define DAC_ENABLE()  (_LATD0 = 0)  // low true
#define DAC_DISABLE() (_LATD0 = 1)
#define CONFIG_DAC()  CONFIG_RD0_AS_DIG_OUTPUT() // chip select
#define DAC_MASK      (0xB000) // OR with this; see below
// *********************************************************************

#define TRI      (0x01) // triangle wave
#define SINE     (0x02) // sine wave
#define SQUARE   (0x03) // square wave

// the menu items
#define WAVEFORM (1)
#define FREQ     (2)
#define AMPLTD   (3)
#define DUTY     (4)
#define LEDSET   (6)

volatile uint16_t u16_period; // period for frequency 
volatile uint16_t u16_amp;
volatile uint8_t u8_waveSelection;
volatile uint16_t u16_duty;
volatile uint16_t u16_position;
volatile uint8_t u8_dutyCount;

// **********************************************************************

/*

MCP4922 DAC

  | A/B   |   BUF   |   GA#   | SHDN# |   D11:D0  |
  | 1     |   0     |   1     | 1     |   000     |
  A/B
      1 = Write to DAC_B
      0 = Write to DAC_A
  BUF
      1 = Buffered
      0 = Unbuffered // SEE DATASHEET
  GA#
      1 = 1x(Vout = Vref * D/4096)
      0 = 0x(Vout = Vref * D/4096)
  SHDN#
      1 = Active out channel
      0 = Shutdown selected out channel
  D11:D0
      data bits
*/

// sine wave lookup table
const uint16_t au16_sinetbl[] = { 0x800,0x864,0x8c8,0x92c,0x98f,0x9f1,0xa52,0xab1,\
                                  0xb0f,0xb6b,0xbc5,0xc1c,0xc71,0xcc3,0xd12,0xd5f,\
                                  0xda7,0xded,0xe2e,0xe6c,0xea6,0xedc,0xf0d,0xf3a,\
                                  0xf63,0xf87,0xfa7,0xfc2,0xfd8,0xfe9,0xff5,0xffd,\
                                  0xfff,0xffd,0xff5,0xfe9,0xfd8,0xfc2,0xfa7,0xf87,\
                                  0xf63,0xf3a,0xf0d,0xedc,0xea6,0xe6c,0xe2e,0xded,\
                                  0xda7,0xd5f,0xd12,0xcc3,0xc71,0xc1c,0xbc5,0xb6b,\
                                  0xb0f,0xab1,0xa52,0x9f1,0x98f,0x92c,0x8c8,0x864,\
                                  0x800,0x79b,0x737,0x6d3,0x670,0x60e,0x5ad,0x54e,\
                                  0x4f0,0x494,0x43a,0x3e3,0x38e,0x33c,0x2ed,0x2a0,\
                                  0x258,0x212,0x1d1,0x193,0x159,0x123,0xf2,0xc5,\
                                  0x9c,0x78,0x58,0x3d,0x27,0x16,0xa,0x2,\
                                  0x0,0x2,0xa,0x16,0x27,0x3d,0x58,0x78,\
                                  0x9c,0xc5,0xf2,0x123,0x159,0x193,0x1d1,0x212,\
                                  0x258,0x2a0,0x2ed,0x33c,0x38e,0x3e3,0x43a,0x494,\
                                  0x4f0,0x54e,0x5ad,0x60e,0x670,0x6d3,0x737,0x79b
                                };

// triangle wave lookup table
const uint16_t au16_tritbl[] = {  0x40,0x80,0xc0,0x100,0x140,0x180,0x1c0,0x200,\
                                  0x240,0x280,0x2c0,0x300,0x340,0x380,0x3c0,0x400,\
                                  0x440,0x480,0x4c0,0x500,0x540,0x580,0x5c0,0x600,\
                                  0x640,0x680,0x6c0,0x700,0x740,0x780,0x7c0,0x800,\
                                  0x83f,0x87f,0x8bf,0x8ff,0x93f,0x97f,0x9bf,0x9ff,\
                                  0xa3f,0xa7f,0xabf,0xaff,0xb3f,0xb7f,0xbbf,0xbff,\
                                  0xc3f,0xc7f,0xcbf,0xcff,0xd3f,0xd7f,0xdbf,0xdff,\
                                  0xe3f,0xe7f,0xebf,0xeff,0xf3f,0xf7f,0xfbf,0xfff,\
                                  0xfbf,0xf7f,0xf3f,0xeff,0xebf,0xe7f,0xe3f,0xdff,\
                                  0xdbf,0xd7f,0xd3f,0xcff,0xcbf,0xc7f,0xc3f,0xbff,\
                                  0xbbf,0xb7f,0xb3f,0xaff,0xabf,0xa7f,0xa3f,0x9ff,\
                                  0x9bf,0x97f,0x93f,0x8ff,0x8bf,0x87f,0x83f,0x800,\
                                  0x7c0,0x780,0x740,0x700,0x6c0,0x680,0x640,0x600,\
                                  0x5c0,0x580,0x540,0x500,0x4c0,0x480,0x440,0x400,\
                                  0x3c0,0x380,0x340,0x300,0x2c0,0x280,0x240,0x200,\
                                  0x1c0,0x180,0x140,0x100,0xc0,0x80,0x40,0x0
                               };

// this array gets values of the current output wave
uint16_t au16_waveValues[130];

// well the spi has to be setup. enough said
inline void configSPI1() {
  SPI1CON1 = SEC_PRESCAL_4_1 | // 4:1 prescale = 60MHz / 4 = 15MHz
             PRI_PRESCAL_1_1 |
             CLK_POL_ACTIVE_HIGH   | //clock active high (CKP = 0)
             SPI_CKE_ON          | //out changes inactive to active (CKE=0)
             SPI_MODE16_ON        | //16-bit mode
             MASTER_ENABLE_ON;     //master mode

  CONFIG_SDO1_TO_RP(RD4_RP);      //use RD4 for SDO
  CONFIG_RD4_AS_DIG_OUTPUT();   //Ensure that this is a digital output
  CONFIG_SCK1OUT_TO_RP(RD5_RP);   //use RD5 for SCLK
  CONFIG_RD5_AS_DIG_INPUT();   //Ensure that this is a digital input
  CONFIG_DAC();       //chip select for MCP41xxx
  DAC_DISABLE();             //disable the chip select
  SPI1STATbits.SPIEN = 1;  //enable SPI mode
}

// timer to fire our interrupt to send our spi
void configTimer2(void) {
  T2CON = T2_SOURCE_INT | // internal clock
          T2_PS_1_1 | // 1 to 1 prescale
          T2_OFF    |
          T2_GATE_OFF;
  PR2 = 0x1c63;
  ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T2);
  T2CONbits.TON = 1;
}

// handles the spi communication
ESOS_USER_INTERRUPT(ESOS_IRQ_PIC24_T2) {
  static uint16_t u16_sendToDAC;
  ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T2);
  DAC_ENABLE(); // DAC CS enable

  if(u8_waveSelection != SQUARE) {
    u16_sendToDAC = au16_waveValues[u16_position] | DAC_MASK;
    if(u16_position >= 0x7E) {
      u16_position = 0x00;
    }
    else {
      u16_position++;
    }
  }
  else {
    if(u8_dutyCount > (u16_duty * 5) / 4 + 1) {
      u16_sendToDAC = 0x0000 | DAC_MASK;
    }
    else {
      u16_sendToDAC = (((uint32_t)0x0FFF * u16_amp) / 30) | DAC_MASK;
    }
    if(u8_dutyCount >= 0x7E) {
      u8_dutyCount = 0;
    }
    u8_dutyCount++;
  }
  if (SPI1STATbits.SPIROV) {
    //clear the error
    SPI1STATbits.SPIROV = 0;
  }
  _SPI1IF = 0; // clears the SPI interupt since new data is fixing to be written
  SPI1BUF = u16_sendToDAC;
  while (!_SPI1IF) { //wait for operation to complete
    _LATB5 = !_LATB5; // I done this so I could see how long it takes to send the SPI data
  }
  DAC_DISABLE(); // DAC CS disable
}

/************************************************************************
 * ESOS TASK
 ************************************************************************
 */

ESOS_USER_TASK(LEDS)  {
  ESOS_TASK_BEGIN();
  while (TRUE) {
    // when binary is set it returns values between 0x00 and 0x06
    // value  LED3  LED2  LED1
    // 000    off   off   off
    // 001    on    off   off
    // 010    off   on    off
    // 011    on    on    off
    // 100    off   off   on
    // 101    on    off   on
    // 110    off   on    on
    // 111    on    on    on
    if(esos_getValue(LEDSET) == 0x07) {
      f15ui_turnLED1ON();
      f15ui_turnLED2ON();
      f15ui_turnLED3ON();
    }
    else if(esos_getValue(LEDSET) == 0x06) {
      f15ui_turnLED1ON();
      f15ui_turnLED2ON();
      f15ui_turnLED3OFF();
    }
    else if(esos_getValue(LEDSET) == 0x05) {
      f15ui_turnLED1ON();
      f15ui_turnLED2OFF();
      f15ui_turnLED3ON();
    }
    else if(esos_getValue(LEDSET) == 0x04) {
      f15ui_turnLED1ON();
      f15ui_turnLED2OFF();
      f15ui_turnLED3OFF();
    }
    else if(esos_getValue(LEDSET) == 0x03) {
      f15ui_turnLED1OFF();
      f15ui_turnLED2ON();
      f15ui_turnLED3ON();
    }
    else if(esos_getValue(LEDSET) == 0x02) {
      f15ui_turnLED1OFF();
      f15ui_turnLED2ON();
      f15ui_turnLED3OFF();
    }
    else if(esos_getValue(LEDSET) == 0x01) {
      f15ui_turnLED1OFF();
      f15ui_turnLED2OFF();
      f15ui_turnLED3ON();
    }
    else {
      f15ui_turnLED1OFF();
      f15ui_turnLED2OFF();
      f15ui_turnLED3OFF();
    }
    ESOS_TASK_WAIT_TICKS(500);
  } // endof while(TRUE)
  ESOS_TASK_END();
} // END OF LEDS TASK

ESOS_USER_TASK(DAC_MENU)  {
  ESOS_TASK_BEGIN();
  static uint16_t u16_oldPeriod; // keep up with any changes
  static uint16_t u16_oldWave; // again for changes
  static uint32_t u32_timerPeriod; // for calculating the new PR2 period
  static uint16_t u16_waveValue; // the scalled value for the wave
  static uint16_t u16_counter; // a counter DUH
  static uint16_t u16_oldAmp; // some more stuff for changes
  u16_oldWave = 0x00; // wave is initially 0 at startup
  u16_oldAmp = 30; // amp is 30 at startup
  u16_counter = 0; // this has to be 0, don't ask questions
  while (TRUE) {
    // first get the waveform
    u8_waveSelection = esos_getSV3CValue(WAVEFORM);
    u16_period = esos_getValue(FREQ);
    u16_duty = esos_getValue(DUTY);
    u16_amp = esos_getValue(AMPLTD);
    // if the wvform is not square, hide the duty cycle menu
    if(u8_waveSelection != SQUARE) {
      esos_hideMenuTitle(DUTY, TRUE);
    }
    else {
      esos_hideMenuTitle(DUTY, FALSE);
    }
    // if any changes happen to the wave or the amp, update the waveform table
    if(u16_oldWave != u8_waveSelection || u16_oldAmp != u16_amp) {
      if(u8_waveSelection == SINE) {
        u16_waveValue = (((uint32_t)au16_sinetbl[u16_counter] * u16_amp) / 30);
        au16_waveValues[u16_counter] = u16_waveValue;
        u16_counter++;
      }
      else if(u8_waveSelection == TRI) {
        u16_waveValue = (((uint32_t)au16_tritbl[u16_counter] * u16_amp) / 30);
        au16_waveValues[u16_counter] = u16_waveValue;
        u16_counter++;
      }
    }
    // once the counter is 128 the wavetable is full
    if(u16_counter >= 128) {
      u16_oldWave = u8_waveSelection;
      u16_oldAmp = u16_amp;
      u16_counter = 0;
    }
    // if the frequency has changed, update the PR2
    if(u16_oldPeriod != u16_period) {
      u16_oldPeriod = u16_period;
      u32_timerPeriod = 0x6EC2F / (uint32_t)u16_period;     //This is Team Apple's super top secret magic formula
      PR2 = (uint16_t)u32_timerPeriod;
    }
    ESOS_TASK_WAIT_TICKS(10); // might need to change this
  } // endof while(TRUE)
  ESOS_TASK_END();
} // END OF DAC_MENU TASK

void user_init(void) {
  // initilize the menu, LCD and UI
  esos_init_menu();
  
  // used for my own curiosity for the SPI transmission
  CONFIG_RB5_AS_DIG_OUTPUT();
  _LATB5 = 1;
  
  u16_position = 0; // initial values
  u8_dutyCount = 0; // initial values
  // user_init() should register at least one user task
  esos_RegisterTask(LEDS); // for change in LED states
  esos_RegisterTask(DAC_MENU); // for change in wave 
  // setup the menu
  esos_create_menu(7);
  // setup the menu
  // see the .h file for details
  esos_insert_menu_title(WAVEFORM, SET, SV3C, "wvform");
  esos_insert_menu_title(FREQ, SET, SVDATA, "freq");
  esos_insert_menu_title(AMPLTD, SET, SVDATA, "ampltd");
  esos_insert_menu_title(DUTY, SET, SVDATA, "duty");
  esos_insert_menu_title(5, READ, NONE, "LM60");
  esos_insert_menu_title(LEDSET, SET, SVDATA, "LEDs");
  esos_insert_menu_title(7, STATIC, NONE, "About");
  esos_setSV3C(WAVEFORM, "tri   ", "sine  ", "square ");
  esos_setSVDATA(FREQ, 64, 2047, FALSE, FALSE);
  esos_setSVDATA(AMPLTD, 0, 30, FALSE, FALSE);
  esos_setSVDATA(DUTY, 0, 100, FALSE, FALSE);
  esos_setSVDATA(5, 0, 100, FALSE, FALSE);
  esos_setSVDATA(LEDSET, 0, 1110, TRUE, TRUE);
  esos_setSensorReading(5, ESOS_SENSOR_CH03);
  esos_setStaticInfo(7, "Developers DIGITAL FUNCTION SYNTHESIZER",
                     "James CPE Andrew EE John CPE Wyatt CPE");
  configSPI1(); // configure the spi
  configTimer2(); // configure timer 2
  esos_setSVDATAValue(DUTY, 50); // set the default duty cycle
  esos_setSVDATAValue(AMPLTD, 30); // set the default ampl
  // enable the interrupts
  ESOS_REGISTER_PIC24_USER_INTERRUPT( ESOS_IRQ_PIC24_T2, ESOS_USER_IRQ_LEVEL1,
                                      _T3Interrupt);
  ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T2);
  init_i2c_task();

} // end user_init()