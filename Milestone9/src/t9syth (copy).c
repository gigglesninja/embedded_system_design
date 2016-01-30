// TEAM APPLE!

// FINAL TASK!!! THANK GOD THIS IS OVER!!!!

// INCLUDEs go here  (First include the main esos.h file)
//      After that, the user can include what they need
//#include "revF15.h"
#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24_spi.h"
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24_i2c.h"
#include "../../include/esos_lcd_menu.h"
#include "../../include/esos_can.h"
#include "../../include/can_config.h"
#include <stdio.h>

/************************************************************************
 * DEFINES
 ************************************************************************
 */

#define DAC_ENABLE()    (_LATD0 = 0)  // low true
#define DAC_DISABLE()   (_LATD0 = 1)
#define CONFIG_DAC()    CONFIG_RD0_AS_DIG_OUTPUT() // chip select
#define DAC_MASK_A      (0x3000) // write to Vout A reg
#define DAC_MASK_B      (0xB000) // write to Vout B reg

#define TRI      (0x01) // triangle wave
#define SINE     (0x02) // sine wave
#define SQUARE   (0x03) // square wave

// teams
#define APPLE   (1)
#define CHERRY  (2)
#define TEACHER (3)

// the menu items
#define TEAM      (1)
#define WAVEFORM  (2)
#define FREQ      (3)
#define AMPLTD    (4)
#define DUTY      (5)
#define LM60      (6)
#define LEDSET    (7)
#define ABOUT     (8)

// message type
#define CAN_WAVE    (0x00)
#define CAN_FREQ    (0x01)
#define CAN_AMP     (0x02)
#define CAN_LM60    (0x03)
#define CAN_DS1631  (0x04)
#define CAN_LED     (0x05)
#define CAN_DUTY    (0x06)

#define ECHO_TIMEOUT  (35000)

#define DS1631ADDR 0x90   //DS1631 address with all pins tied low
#define ACCESS_CONFIG 0xAC
#define CONFIG_COMMAND 0x0C //continuous conversion, 12-bit mode
#define START_CONVERT 0x51
#define READ_TEMP 0xAA

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
      0 = 0x(Vout = 2Vref * D/4096)
  SHDN#
      1 = Active out channel
      0 = Shutdown selected out channel
  D11:D0
      data bits
*/

/************************************************************************
 * GLOBALS
 ************************************************************************
 */

typedef struct ids {
  uint16_t msg_ID;
  uint8_t u8_team;
  uint8_t u8_member;
  uint32_t u32_lastEcho;
  uint8_t u8_wave;
  uint16_t u16_freq;
  uint16_t u16_amp;
  uint16_t u16_duty;
  uint8_t u8_leds;
  uint16_t u16_temp;
  bool active;
  char *psz_netID;
  uint8_t u8_index;
} IDS;

volatile uint16_t u16_period; // period for frequency
volatile uint16_t u16_amp;
volatile uint8_t u8_waveSelection;
volatile uint16_t u16_duty;
volatile uint16_t u16_position;
volatile uint8_t u8_dutyCount;

volatile uint8_t u8_currentMenuIndex;

volatile uint16_t u16_periodDACB; // period for frequency
volatile uint16_t u16_ampDACB;
volatile uint8_t u8_waveSelectionDACB;
volatile uint16_t u16_dutyDACB;
volatile uint16_t u16_positionDACB; // need an A and B
volatile uint8_t u8_dutyCountDACB; // need an A and B

volatile bool b_CANINUSE; // mutex to keep control of CAN

// this array gets values of the current output wave
uint16_t au16_waveDACA[130]; // DACA
uint16_t au16_waveDACB[130]; // DACB

IDS ids[10] = {
//"Name" / ID     team  member  lastEcho wave freq amp duty leds temp active    ID      index
  /*JD*/ {0x120,   1,     1,     0x0000,   1,   64,  30,  50,   0,   0,    0,   "jdf249",    1},
  /*WG*/ {0x140,   1,     2,     0x0000,   1,   64,  30,  50,   0,   0,    0,   "jwg254",    2},
  /*AH*/ {0x160,   1,     3,     0x0000,   1,   64,  30,  50,   0,   0,    0,   "arh426",    3},
  /*JT*/ {0x180,   1,     4,     0x0000,   1,   64,  30,  50,   0,   0,    0,   "jct322",    4},
  /*BB*/ {0x220,   2,     1,     0x0000,   1,   64,  30,  50,   0,   0,    0,   "bab334",    5},
  /*AC*/ {0x240,   2,     2,     0x0000,   1,   64,  30,  50,   0,   0,    0,   "ajc269",    6},
  /*JC*/ {0x260,   2,     3,     0x0000,   1,   64,  30,  50,   0,   0,    0,   "jc1485",    7},
  /*GH*/ {0x280,   2,     4,     0x0000,   1,   64,  30,  50,   0,   0,    0,   "gh403 ",    8},
  /*DT*/ {0x300,   3,     1,     0x0000,   1,   64,  30,  50,   0,   0,    0,   "rat56 ",    9},
  /*JB*/ {0x320,   3,     2,     0x0000,   1,   64,  30,  50,   0,   0,    0,   "jwb   ",    10},
};

char *psz_netIDs[4];

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


/************************************************************************
 * FUNCTIONS
 ************************************************************************
 */

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
  if(MY_ID == 0) {
    CONFIG_SCK1OUT_TO_RP(RD1_RP);   //use RD5 for SCLK
    CONFIG_RD1_AS_DIG_INPUT();   //Ensure that this is a digital input
  }
  else {
    CONFIG_SCK1OUT_TO_RP(RD5_RP);   //use RD5 for SCLK
    CONFIG_RD5_AS_DIG_INPUT();   //Ensure that this is a digital input
  }
  CONFIG_DAC();       //chip select for MCP41xxx
  DAC_DISABLE();             //disable the chip select
  SPI1STATbits.SPIEN = 1;  //enable SPI mode
}

// turns the incoming index int into a char
char ID_to_Char(uint8_t u8_id) {
  if(u8_id == 0) {
    return '0';
  }
  else if(u8_id == 1) {
    return '1';
  }
  else if(u8_id == 2) {
    return '2';
  }
  else if(u8_id == 3) {
    return '3';
  }
  else if(u8_id == 4) {
    return '4';
  }
  else if(u8_id == 5) {
    return '5';
  }
  else if(u8_id == 6) {
    return '6';
  }
  else if(u8_id == 7) {
    return '7';
  }
  else if(u8_id == 8) {
    return '8';
  }
  else if(u8_id == 9) {
    return '9';
  }
  else {
    return 'B';
  }
}


/************************************************************************
 * INTERRUPTS / TIMERS
 ************************************************************************
 */

// ********************** D A C - A - O U T P U T ***********************
// timer for DACA
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
  static uint16_t u16_sendToDAC; // value to send to dac
  ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T2);
  DAC_ENABLE(); // DAC CS enable
  if(u8_waveSelection != SQUARE) {
    u16_sendToDAC = au16_waveDACA[u16_position] | DAC_MASK_A;
    if(u16_position >= 0x7E) {
      u16_position = 0x00;
    }
    else {
      u16_position++;
    }
  }
  else {
    if(u8_dutyCount > (u16_duty * 5) / 4 + 1) {
      u16_sendToDAC = 0x0000 | DAC_MASK_A;
    }
    else {
      u16_sendToDAC = (((uint32_t)0x0FFF * u16_amp) / 30) | DAC_MASK_A;
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
  while (!_SPI1IF); //wait for operation to complete
  DAC_DISABLE(); // DAC CS disable
}

// ********************** D A C - B - O U T P U T ***********************
// timer for DACB
void configTimer3(void) {
  T3CON = T3_SOURCE_INT | // internal clock
          T3_PS_1_1 | // 1 to 1 prescale
          T3_OFF    |
          T3_GATE_OFF;
  PR3 = 0x1c63;
  ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T3);
  T3CONbits.TON = 1;
}
// handles the spi communication
ESOS_USER_INTERRUPT(ESOS_IRQ_PIC24_T3) {
  static uint16_t u16_sendToDAC;
  ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T3);
  DAC_ENABLE(); // DAC CS enable
  if(u8_waveSelectionDACB != SQUARE) {
    u16_sendToDAC = au16_waveDACB[u16_positionDACB] | DAC_MASK_B;
    if(u16_positionDACB >= 0x7E) {
      u16_positionDACB = 0x00;
    }
    else {
      u16_positionDACB++;
    }
  }
  else {
    if(u8_dutyCount > (u16_duty * 5) / 4 + 1) {
      u16_sendToDAC = 0x0000 | DAC_MASK_B;
    }
    else {
      u16_sendToDAC = (((uint32_t)0x0FFF * u16_amp) / 30) | DAC_MASK_B;
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
  while (!_SPI1IF); //wait for operation to complete
  DAC_DISABLE(); // DAC CS disable
}

//***********************************************************************

/************************************************************************
 * ESOS TASK
 ************************************************************************
 */

// CHILD TASK THAT WILL HANDLE SETTING TEAM MEMBERS
ESOS_CHILD_TASK(members, bool stay, uint8_t team) {
  ESOS_TASK_BEGIN();
  esos_lcd_clearScreen(); // clear the screen
  static bool b_stay, b_update;
  static uint8_t numUsers, teamMembers;
  static int i_counter;
  numUsers = 0; // initial users is 0
  esos_lcd_setCursor(ROW_ONE, 1 );
  b_stay = stay; // this should be true
  if(team == 3) { // if teachers there are only 2
    i_counter = 8; // the counter needs to be set to the lowest index of team 3
    teamMembers = 10; // team members need to be set to highest + 1
  }
  else if(team == 2) {
    i_counter = 4; // counter needs to be set to lowest index of team 2
    teamMembers = 8; // team members need to be set to highest + 1
  }
  else {
    i_counter = 0; // counter set to lowest index of team 1
    teamMembers = 4; // team members set to highest + 1
  }
  while(i_counter <
        teamMembers) { // loop though and find any active users of the team
    if(ids[i_counter].active) {
      psz_netIDs[numUsers] = ids[i_counter].psz_netID; // add active users to the list
      numUsers++; // increase the number of users
    }
    i_counter++;
  }
  i_counter = 0;
  b_update = true; // set update to be true initially
  if(numUsers == 0) { // if no users send a message
    b_stay = FALSE;
    esos_lcd_setCursor(ROW_ONE, 1 );
    ESOS_TASK_WAIT_LCD_WRITE_DATA('N');
    ESOS_TASK_WAIT_LCD_WRITE_DATA('O');
    ESOS_TASK_WAIT_LCD_WRITE_DATA('N');
    ESOS_TASK_WAIT_LCD_WRITE_DATA('E');
    ESOS_TASK_WAIT_TICKS(2000);
  }
  while(b_stay) { // wait until a selection is made
    if(f15ui_isRpgTurningCCW() || f15ui_isSW2Pressed()) { // menu goes up
      i_counter++;
      b_update = TRUE; // update the screen
      if(i_counter > numUsers - 1) {
        i_counter = numUsers - 1;
        b_update = FALSE;
      }
    }
    else if(f15ui_isRpgTurningCW() || f15ui_isSW1Pressed()) { // menu goes down
      i_counter--;
      b_update = TRUE; // update the screen
      // if counter is going under item count, don't update
      if(i_counter < 0) {
        i_counter = 0;
        b_update = FALSE;
      }
    }
    if(b_update) {
      esos_lcd_setCursor(ROW_ONE, 1 );
      ESOS_TASK_WAIT_LCD_WRITE_DATA(psz_netIDs[i_counter][0]);
      ESOS_TASK_WAIT_LCD_WRITE_DATA(psz_netIDs[i_counter][1]);
      ESOS_TASK_WAIT_LCD_WRITE_DATA(psz_netIDs[i_counter][2]);
      ESOS_TASK_WAIT_LCD_WRITE_DATA(psz_netIDs[i_counter][3]);
      ESOS_TASK_WAIT_LCD_WRITE_DATA(psz_netIDs[i_counter][4]);
      ESOS_TASK_WAIT_LCD_WRITE_DATA(psz_netIDs[i_counter][5]);
      // if the menu is at the top display on the down arrow
      if(i_counter == 0) {
        esos_lcd_setCursor(ROW_ONE, 8 );
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
        esos_lcd_setCursor(ROW_TWO, 8 );
        ESOS_TASK_WAIT_LCD_WRITE_DATA(DOWN);
      }
      // if the menu is at the bottom display only the up arrow
      else if(i_counter == numUsers - 1) {
        esos_lcd_setCursor(ROW_ONE, 8 );
        ESOS_TASK_WAIT_LCD_WRITE_DATA(UP);
        esos_lcd_setCursor(ROW_TWO, 8 );
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
      }
      // otherwise display them both
      else {
        esos_lcd_setCursor(ROW_ONE, 8 );
        ESOS_TASK_WAIT_LCD_WRITE_DATA(UP);
        esos_lcd_setCursor(ROW_TWO, 8 );
        ESOS_TASK_WAIT_LCD_WRITE_DATA(DOWN);
      }
      b_update = FALSE;
    }
    ESOS_TASK_WAIT_TICKS(100);
    if(f15ui_isSW3Pressed()) {
      b_stay = FALSE;
      int i = 0;
      while(i < 10) {
        if(psz_netIDs[i_counter] == ids[i].psz_netID) {
          u8_currentMenuIndex = i;
          char id;
          if(i == MY_ID) {
            id = ID_to_Char(0);
          }
          else {
            id = ID_to_Char(ids[i].u8_index);
          }
          esos_insert_menu_title(WAVEFORM, SET, SV3C, "wvform", id);
          esos_insert_menu_title(FREQ, SET, SVDATA, "freq", id);
          esos_insert_menu_title(AMPLTD, SET, SVDATA, "ampltd", id);
          esos_insert_menu_title(DUTY, SET, SVDATA, "duty", id);
          esos_insert_menu_title(LM60, READ, NONE, "LM60", id);
          esos_insert_menu_title(LEDSET, SET, SVDATA, "LEDs", id);
          esos_setSV3C_VALUE(WAVEFORM, ids[i].u8_wave);
          esos_setSVDATAValue(FREQ, ids[i].u16_freq); // set the default duty cycle
          esos_setSVDATAValue(AMPLTD, ids[i].u16_amp); // set the default ampl
          esos_setSVDATAValue(DUTY, ids[i].u16_duty); // set the default duty cycle
          esos_setSVDATAValue(LEDSET, ids[i].u8_leds); // set the default duty cycle
        }
        i++;
      }
      i_counter = 0;
    }
  }
  ESOS_TASK_END();
}

// TASK for the setting team
ESOS_USER_TASK(SET_TEAM)  {
  ESOS_TASK_BEGIN();
  clearNewTeam();
  static ESOS_TASK_HANDLE th_child;
  while (TRUE) {
    ESOS_TASK_WAIT_UNTIL(newTeam()); // wait for a new team to be set
    ESOS_ALLOCATE_CHILD_TASK(th_child);
    if(esos_getSV3CValue(TEAM) == APPLE) {
      ESOS_TASK_SPAWN_AND_WAIT(th_child, members, TRUE, 1);
    }
    if(esos_getSV3CValue(TEAM) == CHERRY) {
      ESOS_TASK_SPAWN_AND_WAIT(th_child, members, TRUE, 2);
    }
    if(esos_getSV3CValue(TEAM) == TEACHER) {
      ESOS_TASK_SPAWN_AND_WAIT(th_child, members, TRUE, 3);
    }
    clearNewTeam(); // clear the flag
  } // endof while(TRUE)
  ESOS_TASK_END();
} // END OF team TASK

// TASK for the LEDS
ESOS_USER_TASK(LEDS)  {
  ESOS_TASK_BEGIN();
  static union64 u64_data;
  static u8_oldLED;
  u8_oldLED = 0x00;
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
    if(esos_getValue(LEDSET) == 0x07) { // ALL LEDS ON
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
    if(u8_oldLED != esos_getValue(LEDSET)) {
      ESOS_TASK_WAIT_UNTIL(!b_CANINUSE);
      b_CANINUSE = 1;
      u64_data.u32[0] = (uint32_t)esos_getValue(LEDSET);
      msgBuf[0].data = u64_data;
      formatStandardDataFrameECAN(&msgBuf[0],
                                  calcMsgID(u8_currentMenuIndex) | CAN_LED, 1);
      startTxECAN1(0);  //start transmission of buffer 0
      ESOS_TASK_WAIT_WHILE(getTxInProgressECAN1(0));
      b_CANINUSE = 0; // release the CAN
      u8_oldLED = esos_getValue(LEDSET);
    }
    ESOS_TASK_WAIT_TICKS(500);
  } // endof while(TRUE)
  ESOS_TASK_END();
} // END OF LEDS TASK

// TASK UPDATES THE TEMP
ESOS_USER_TASK(UPDATE_TEMP) {
  ESOS_TASK_BEGIN();
  while(TRUE) {
    esos_setSensorReading(LM60, 0, ids[u8_currentMenuIndex].u16_temp);
    ESOS_TASK_WAIT_TICKS(1000);
  }
  ESOS_TASK_END();
}

// DAC A SHOULD BE OUR BOARD
ESOS_USER_TASK(DACA)  {
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
    u8_waveSelection = ids[MY_ID].u8_wave;
    u16_period = ids[MY_ID].u16_freq;
    u16_duty = ids[MY_ID].u16_duty;
    u16_amp = ids[MY_ID].u16_amp;
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
        au16_waveDACA[u16_counter] = u16_waveValue;
        u16_counter++;
      }
      else if(u8_waveSelection == TRI) {
        u16_waveValue = (((uint32_t)au16_tritbl[u16_counter] * u16_amp) / 30);
        au16_waveDACA[u16_counter] = u16_waveValue;
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
      u32_timerPeriod = 0x6EC2F / (uint32_t)
                        u16_period;     //This is Team Apple's super top secret magic formula
      PR2 = (uint16_t)u32_timerPeriod;
    }
    ESOS_TASK_WAIT_TICKS(10); // might need to change this
  } // endof while(TRUE)
  ESOS_TASK_END();
} // END OF DAC_MENU TASK

// DACB SHOULD BE CURRENT BOARD SELECTED IN MENU
ESOS_USER_TASK(DACB)  {
  ESOS_TASK_BEGIN();
  static union64 u64_data;
  static uint16_t u16_oldPeriod; // keep up with any changes
  static uint16_t u16_oldWave; // again for changes
  static uint32_t u32_timerPeriod; // for calculating the new PR2 period
  static uint16_t u16_waveValue; // the scalled value for the wave
  static uint16_t u16_counter; // a counter DUH
  static uint16_t u16_oldAmp; // some more stuff for changes
  static bool b_change;
  b_change = FALSE;
  u16_oldWave = 0x00; // wave is initially 0 at startup
  u16_oldAmp = 30; // amp is 30 at startup
  u16_counter = 0; // this has to be 0, don't ask questions
  while (TRUE) {
    // first get the waveform
    u8_waveSelectionDACB = esos_getSV3CValue(WAVEFORM);
    u16_period = esos_getValue(FREQ);
    u16_duty = esos_getValue(DUTY);
    u16_amp = esos_getValue(AMPLTD);
    // if the wvform is not square, hide the duty cycle menu
    if(u8_waveSelectionDACB != SQUARE) {
      esos_hideMenuTitle(DUTY, TRUE);
      ids[MY_ID].u16_duty = u16_duty;
    }
    else {
      esos_hideMenuTitle(DUTY, FALSE);
    }
    // if any changes happen to the wave or the amp, update the waveform table
    if(u16_oldWave != u8_waveSelectionDACB || u16_oldAmp != u16_amp) {
      if(u8_waveSelectionDACB == SINE) {
        u16_waveValue = (((uint32_t)au16_sinetbl[u16_counter] * u16_amp) / 30);
        au16_waveDACB[u16_counter] = u16_waveValue;
        u16_counter++;
      }
      else if(u8_waveSelectionDACB == TRI) {
        u16_waveValue = (((uint32_t)au16_tritbl[u16_counter] * u16_amp) / 30);
        au16_waveDACB[u16_counter] = u16_waveValue;
        u16_counter++;
      }
      else {
        u16_oldWave = u8_waveSelectionDACB;
        b_change = TRUE;
        u16_counter = 129;
      }
    }
    // once the counter is 128 the wavetable is full
    if(u16_counter >= 128) {
      u16_oldWave = u8_waveSelectionDACB;
      u16_oldAmp = u16_amp;
      u16_counter = 0;
      b_change = TRUE;
    }
    // if the frequency has changed, update the PR2
    if(u16_oldPeriod != u16_period) {
      u16_oldPeriod = u16_period;
      u32_timerPeriod = 0x6EC2F / (uint32_t)
                        u16_period;     //This is Team Apple's super top secret magic formula
      PR3 = (uint16_t)u32_timerPeriod;
      b_change = TRUE;
    }
    if(b_change) {
      b_change = FALSE;
      ids[u8_currentMenuIndex].u8_wave = u8_waveSelectionDACB;
      ids[u8_currentMenuIndex].u16_freq = u16_period;
      ids[u8_currentMenuIndex].u16_amp = u16_amp;
      ESOS_TASK_WAIT_UNTIL(!b_CANINUSE);
      b_CANINUSE = 1;
      u64_data.u32[0] = (uint32_t)u8_waveSelectionDACB;
      msgBuf[0].data = u64_data;
      formatStandardDataFrameECAN(&msgBuf[0],
                                  calcMsgID(u8_currentMenuIndex) | CAN_WAVE, 2);
      startTxECAN1(0);  //start transmission of buffer 0
      ESOS_TASK_WAIT_WHILE(getTxInProgressECAN1(0));
      u64_data.u32[0] = (uint32_t)u16_amp;
      msgBuf[0].data = u64_data;
      formatStandardDataFrameECAN(&msgBuf[0],
                                  calcMsgID(u8_currentMenuIndex) | CAN_AMP, 2);
      startTxECAN1(0);  //start transmission of buffer 0
      ESOS_TASK_WAIT_WHILE(getTxInProgressECAN1(0));
      u64_data.u32[0] = (uint32_t)u16_period;
      msgBuf[0].data = u64_data;
      formatStandardDataFrameECAN(&msgBuf[0],
                                  calcMsgID(u8_currentMenuIndex) | CAN_FREQ, 2);
      startTxECAN1(0);  //start transmission of buffer 0
      ESOS_TASK_WAIT_WHILE(getTxInProgressECAN1(0));
      b_CANINUSE = 0; // release the CAN
    }
    ESOS_TASK_WAIT_TICKS(50); // might need to change this
  } // endof while(TRUE)
  ESOS_TASK_END();
} // END OF DAC_MENU TASK

// task that handles the CAN_RX
ESOS_USER_TASK(CAN_RX)  {
  ESOS_TASK_BEGIN();
  static uint8_t u8_rx_buff_id;
  static uint16_t u16_msg_ID, u16_data;
  static uint32_t u32_currentTick;
  while (TRUE) {
    ESOS_TASK_WAIT_UNTIL(_DMA1IF); // blocks until the DMA1 interupt is ready.
    _DMA1IF = 0;   //RX message accepted
    u8_rx_buff_id = GET_FIFO_READBUFFER_ECAN1();
    u16_msg_ID = msgBuf[u8_rx_buff_id].w0.SID;
    u32_currentTick = esos_GetSystemTick();
    u16_data = msgBuf[u8_rx_buff_id].data.u16[0];
    int i = 0;
    while(i <= NUM_OF_IDS) { // loop though ID's to filter the messsage type
      if((u16_msg_ID & 0xFF0) == ids[i].msg_ID) { // find the correct MSG_ID index
        ids[i].u32_lastEcho = u32_currentTick; // get echo tick
        if((u16_msg_ID & 0x00F) == CAN_WAVE) { // WAVE
          ids[i].u8_wave = (uint8_t)u16_data;
          if(u8_currentMenuIndex == i) {
            esos_setSV3C_VALUE(WAVEFORM, ids[i].u8_wave);
          }
        }
        else if((u16_msg_ID & 0x00F) == CAN_FREQ) { // FREQ
          ids[i].u16_freq = u16_data;
          if(u8_currentMenuIndex == i) { // if current index we need to update the data
            esos_setSVDATAValue(FREQ, ids[i].u16_freq); // set the default duty cycle
          }
        }
        else if((u16_msg_ID & 0x00F) == CAN_AMP) { // AMP
          ids[i].u16_amp = u16_data;
          if(u8_currentMenuIndex == i) { // if current index we need to update the data
            esos_setSVDATAValue(AMPLTD, ids[i].u16_amp); // set the default duty cycle
          }
        }
        else if((u16_msg_ID & 0x00F) == CAN_LM60) { // LM60
          ids[i].u16_temp = u16_data;
        }
        else if((u16_msg_ID & 0x00F) == CAN_DS1631) { // DS1631
          if(u16_data != 0x7F) {
            ids[i].u16_temp = u16_data;
          }
        }
        else if((u16_msg_ID & 0x00F) == CAN_LED) { // LEDs
          ids[i].u8_leds = (uint8_t)u16_data;
          if(u8_currentMenuIndex == i) { // if current index we need to update the data
            esos_setSVDATAValue(LEDSET, ids[i].u8_leds); // set the default duty cycle
          }
        }
      }
      i++;
    }
    clrRxFullFlagECAN1(u8_rx_buff_id);
  }
  ESOS_TASK_END();
}

// task that handles CAN_TX
ESOS_USER_TASK(CAN_TEMP_TX)  {
  ESOS_TASK_BEGIN();
  static union64 u64_data;
  static uint16_t u16_data;
  static uint8_t u8_hi, u8_lo;
  static bool b_LM60;
  b_LM60 = TRUE;
  while (TRUE) {
    ESOS_TASK_WAIT_UNTIL(!b_CANINUSE); // wait until can is avaliable
    b_CANINUSE = 1; // lock the CAN
    if(b_LM60) { // For LM60 reading
      ESOS_USE_ADC_12_BIT(TRUE); // since we use 1 channel use 12 bits
      ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(ESOS_SENSOR_CH03,
                                         ESOS_SENSOR_VREF_3V3); // wait and setup sensor
      ESOS_TASK_WAIT_SENSOR_READ(u16_data, ESOS_SENSOR_AVG64,
                                 ESOS_SENSOR_FORMAT_VOLTAGE); // get the ADC reading
      ESOS_TASK_WAIT_UNTIL(ESOS_SENSOR_CLOSE()); // close the sensor
      u64_data.u32[0] = (uint32_t)u16_data;
      msgBuf[0].data = u64_data;
      //format Buffer 0 for TX with SID=MSG_ID, data length = 8 bytes
      formatStandardDataFrameECAN(&msgBuf[0], calcMsgID(MY_ID) | CAN_LM60, 2);
    }
    else { // for DS1631 reading                              NEEDS WORK
      ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
      ESOS_TASK_WAIT_ON_WRITE1I2C1(DS1631ADDR, START_CONVERT);
      ESOS_TASK_WAIT_TICKS(10); // wait to ensure DS1631 is setup
      ESOS_TASK_WAIT_ON_WRITE1I2C1(DS1631ADDR, READ_TEMP);
      ESOS_TASK_WAIT_ON_READ2I2C1(DS1631ADDR, u8_hi, u8_lo);
      u16_data =  u8_hi << 8 | u8_lo; // combine hi and lo
      if(!(u16_data ^ 0xFFFF)) { // if DS1631 is not connected it will read 0xFFFF
        u16_data = 0x007F; // report 7F
      }









      
      // need to do conversion for this











      ESOS_TASK_SIGNAL_AVAILABLE_I2C();
      u64_data.u32[0] = (uint32_t)u16_data;
      msgBuf[0].data = u64_data;
      //format Buffer 0 for TX with SID=MSG_ID, data length = 8 bytes
      formatStandardDataFrameECAN(&msgBuf[0], calcMsgID(MY_ID) | CAN_DS1631, 2);
    }
    ids[MY_ID].u16_temp = u16_data;
    startTxECAN1(0);  //start transmission of buffer 0
    ESOS_TASK_WAIT_WHILE(getTxInProgressECAN1(0));
    b_CANINUSE = 0; // release the CAN
    ESOS_TASK_WAIT_TICKS(5000);
    b_LM60 = !b_LM60;
  }
  ESOS_TASK_END();
}

// MONITORS SYSTEM TICKS TO BUILD LIST
ESOS_USER_TASK(BUILD_LIST)  {
  ESOS_TASK_BEGIN();
  while (TRUE) {
    int i = 0;
    while(i <= NUM_OF_IDS) { // loop though IDS
      if(((ids[i].u32_lastEcho + ECHO_TIMEOUT) > esos_GetSystemTick())
          || i == MY_ID) {
        ids[i].active = TRUE; // set active if not timed out
      }
      else {
        ids[i].active = FALSE; // set inactive if timed out
      }
      i++;
    }
    ESOS_TASK_WAIT_TICKS(8000);
  }
  ESOS_TASK_END();
}

void user_init(void) {
  esos_init_menu(); // initilize the menu, LCD and UI
  configECAN1(); // init can
  CHANGE_MODE_ECAN1(ECAN_MODE_NORMAL); // setup the CAN
  esos_pic24_configI2C1(400); // setup the I2C
  // used for my own curiosity for the SPI transmission
  CONFIG_RD2_AS_DIG_OUTPUT();
  _LATD2 = 1;
  u16_position = 0; // initial values
  u8_dutyCount = 0; // initial values
  u16_positionDACB = 0; // initial values
  u8_dutyCountDACB = 0; // initial values
  b_CANINUSE = 0;
  esos_create_menu(8); // setup the menu
  // see the .h file for details
  esos_insert_menu_title(TEAM, SET, SV3C, "team", '0');
  esos_insert_menu_title(WAVEFORM, SET, SV3C, "wvform", '0');
  esos_insert_menu_title(FREQ, SET, SVDATA, "freq", '0');
  esos_insert_menu_title(AMPLTD, SET, SVDATA, "ampltd", '0');
  esos_insert_menu_title(DUTY, SET, SVDATA, "duty", '0');
  esos_insert_menu_title(LM60, READ, NONE, "LM60", '0');
  esos_insert_menu_title(LEDSET, SET, SVDATA, "LEDs", '0');
  esos_insert_menu_title(ABOUT, STATIC, NONE, "About", '0');
  esos_setSV3C(TEAM, "APPLE  ", "CHERRY  ", "TEACHER");
  esos_setSV3C(WAVEFORM, "tri   ", "sine  ", "square ");
  esos_setSVDATA(FREQ, 64, 2047, FALSE, FALSE);
  esos_setSVDATA(AMPLTD, 0, 30, FALSE, FALSE);
  esos_setSVDATA(DUTY, 0, 100, FALSE, FALSE);
  esos_setSVDATA(LM60, 0, 100, FALSE, FALSE);
  esos_setSVDATA(LEDSET, 0, 1110, TRUE, TRUE);
  esos_setStaticInfo(ABOUT, "Developers DIGITAL FUNCTION SYNTHESIZER",
                     "James CPE Andrew EE John CPE Wyatt CPE");
  configSPI1(); // configure the spi
  configTimer2(); // configure timer 2
  configTimer3(); // configure timer 3
  esos_setSVDATAValue(DUTY, 50); // set the default duty cycle
  esos_setSVDATAValue(AMPLTD, 30); // set the default ampl
  esos_setSV3C_VALUE(TEAM, 1);
  esos_setSV3C_VALUE(WAVEFORM, 1);
  u8_currentMenuIndex = MY_ID; // initial index is set to MY_ID
  // enable the interrupts
  ESOS_REGISTER_PIC24_USER_INTERRUPT( ESOS_IRQ_PIC24_T2, ESOS_USER_IRQ_LEVEL1,
                                      _T2Interrupt);
  ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T2);
  ESOS_REGISTER_PIC24_USER_INTERRUPT( ESOS_IRQ_PIC24_T3, ESOS_USER_IRQ_LEVEL1,
                                      _T3Interrupt);
  ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T3);
  esos_RegisterTask(LEDS); // for change in LED states
  esos_RegisterTask(DACA); // for change in wave
  esos_RegisterTask(DACB); // for change in wave
  esos_RegisterTask(CAN_RX); // CAN RX
  esos_RegisterTask(CAN_TEMP_TX); // CAN TX
  esos_RegisterTask(SET_TEAM); // TEAM SETUP
  esos_RegisterTask(BUILD_LIST); // LIST
  esos_RegisterTask(UPDATE_TEMP); // UPDATES THE TEMP
} // end user_init()