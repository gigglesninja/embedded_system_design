#include    "esos_f15_ui.h"
#include    <stdlib.h>

// ******** G L O B A L S ***************

volatile st_esos_f15UiData s_data;

/// ******* P R I V A T E   M A C R O S **************
// // for checking the states of the switches, leds, and RPG
#define _IS_SW1_PRESSED   (s_data.b_SW1Pressed)
#define _IS_SW2_PRESSED   (s_data.b_SW2Pressed)
#define _IS_SW3_PRESSED   (s_data.b_SW3Pressed)
#define _IS_SW1_RELEASED  (!s_data.b_SW1Pressed)
#define _IS_SW2_RELEASED  (!s_data.b_SW2Pressed)
#define _IS_SW3_RELEASED  (!s_data.b_SW3Pressed)
#define _IS_LED1_ON       (s_data.b_Led1On)
#define _IS_LED2_ON       (s_data.b_Led2On)
#define _IS_LED3_ON       (s_data.b_Led3On)
#define _IS_LED1_OFF      (!s_data.b_Led1On)
#define _IS_LED2_OFF      (!s_data.b_Led2On)
#define _IS_LED3_OFF      (!s_data.b_Led3On)
#define _IS_RPGA_HIGH     (RPGA)
#define _IS_RPGB_HIGH     (RPGB)
#define _IS_RPGA_LOW      (!RPGA)
#define _IS_RPGB_LOW      (!RPGB)

// // macros that point to the functions to turn LEDx on and off
#define _TURN_LED1_ON    (__turnLED1_ON())
#define _TURN_LED2_ON    (__turnLED2_ON())
#define _TURN_LED3_ON    (__turnLED3_ON())
#define _TURN_LED1_OFF   (__turnLED1_OFF())
#define _TURN_LED2_OFF   (__turnLED2_OFF())
#define _TURN_LED3_OFF   (__turnLED3_OFF())

// // macros that will toggle a LED state
#define _TOGGLE_LED1     (__toggleLED1())
#define _TOGGLE_LED2     (__toggleLED2())
#define _TOGGLE_LED3     (__toggleLED3())

#define GREENON          (false)
#define GREENOFF         (true)
#define RPGREV           (12)

// // // P R I V A T E    P R O T O T Y P E S ***********************************/

void __turnLED1_ON(void);
void __turnLED2_ON(void);
void __turnLED3_ON(void);
void __turnLED1_OFF(void);
void __turnLED2_OFF(void);
void __turnLED3_OFF(void);
void __setSW1_state(void);
void __setSW2_state(void);
void __setSW3_state(void);
void __toggleLED1(void);
void __toggleLED2(void);
void __toggleLED3(void);

/****************************************************************
** F U N C T I O N S
****************************************************************/

//variables to store starting turns
uint8_t u8_currentTurns = 0;
uint8_t u8_currentTurnsCW = 0;
uint8_t u8_currentTurnsCCW = 0;

bool b_dummyBit =
  false;  // just a bit to toggle while the rev functions are waiting on the
// user to finish the rotations

// returns true when RPG makes i8_rev turns
bool _RPGRev(uint8_t u8_rev) {
  if(!u8_currentTurns) { // if the current turns value is 0, set it to the current RPG value
    u8_currentTurns = s_data.u8_valueROT;
  }
  // our code increments the RPG by 2 - 3 on each turn. if we shift left by 2 it will equal
  // 1 turn per 2-3 increments
  while(_RPGRevCW(u8_rev) || _RPGRevCCW(u8_rev)) {
    b_dummyBit = !b_dummyBit;
  }
  u8_currentTurns = 0;
  return true;
}
// returns true when RPG makes i8_rev turns CW
bool _RPGRevCW(uint8_t u8_rev) {
  if(!u8_currentTurnsCW) { // if the current turns value is 0, set it to the current RPG value
    u8_currentTurnsCW = s_data.u8_valueROT;
  }
  // our code increments the RPG by 2 - 3 on each turn. if we shift left by 2 it will equal
  // 1 turn per 2-3 increments
  while(((u8_currentTurnsCW - s_data.u8_valueROT) >> 1) < u8_rev) {
    b_dummyBit = !b_dummyBit;
  }
  u8_currentTurnsCW = 0;
  return true;
}
// returns true when RPG makes i8_rev turns CCW
bool _RPGRevCCW(uint8_t u8_rev) {
  if(!u8_currentTurnsCCW) { // if the current turns value is 0, set it to the current RPG value
    u8_currentTurnsCCW = s_data.u8_valueROT;
  }
  // our code increments the RPG by 2 - 3 on each turn. if we shift left by 2 it will equal
  // 1 turn per 2-3 increments
  while(((u8_currentTurnsCCW + s_data.u8_valueROT) >> 1) < u8_rev) {
    b_dummyBit = !b_dummyBit;
  }
  u8_currentTurnsCCW = 0;
  return true;
}

//*******Turns LED on and OFFF***********************************/
void __turnLED1_ON(void) {
  if(!s_data.b_Led1On) {
    s_data.b_Led1On = ON;
    LED1 = ON;
  }
}

void __turnLED2_ON(void) {
  if(!s_data.b_Led2On) {
    s_data.b_Led2On = ON;
    LED2 = ON;
  }
}

void __turnLED3_ON(void) {
  if(!s_data.b_Led3On) {
    s_data.b_Led3On = ON;
    LED3 = GREENON;
  }
}

void __turnLED1_OFF(void) {
  if(s_data.b_Led1On) {
    s_data.b_Led1On = OFF;
    LED1 = OFF;
  }
}

void __turnLED2_OFF(void) {
  if(s_data.b_Led2On) {
    s_data.b_Led2On = OFF;
    LED2 = OFF;
  }
}

void __turnLED3_OFF(void) {
  if(s_data.b_Led3On) {
    s_data.b_Led3On = OFF;
    LED3 = GREENOFF;
  }
}

// ************SET SWITCH PRESSED STATE**********

void __setSW1_state() {
  if(SW1_PRESSED() ) {
    s_data.b_SW1Pressed = true;
  }
  else {
    s_data.b_SW1Pressed = false;
  }
}

void __setSW2_state() {
  if(SW2_PRESSED() ) {
    s_data.b_SW2Pressed = true;
  }
  else {
    s_data.b_SW2Pressed = false;
  }
}

void __setSW3_state() {
  if(SW3_PRESSED() ) {
    s_data.b_SW3Pressed = true;
  }
  else {
    s_data.b_SW3Pressed = false;
  }
}

// //************TOGGLE THE LEDS********************
void __toggleLED1(void) {
  s_data.b_Led1On = !s_data.b_Led1On;
  LED1 = !LED1;
}

void __toggleLED2(void) {
  s_data.b_Led2On = !s_data.b_Led2On;
  LED2 = !LED2;
}

void __toggleLED3(void) {
  s_data.b_Led3On = !s_data.b_Led3On;
  LED3 = !LED3;
}

// **********************************************
// the Pressed functions will run every 10 seconds in the ESOS user task
// this will set the s_data values to false the buttons are not pressed
bool f15ui_isSW1Pressed() {
  if(SW1_PRESSED() ) {
    DELAY_MS(DEBOUNCE_DLY); //allow for a small delay to sample the switch
    __setSW1_state();
    return s_data.b_SW1Pressed;
  }
  else {
    __setSW1_state();
    return s_data.b_SW1Pressed;
  }
}

bool f15ui_isSW2Pressed() {
  if(SW2_PRESSED() ) {
    DELAY_MS(DEBOUNCE_DLY);
    __setSW2_state();
    return s_data.b_SW2Pressed;
  }
  else {
    __setSW2_state();
    return s_data.b_SW2Pressed;
  }
}

bool f15ui_isSW3Pressed() {
  if(SW2_PRESSED() ) {
    DELAY_MS(DEBOUNCE_DLY);
    __setSW3_state();
    return s_data.b_SW3Pressed;
  }
  else {
    __setSW3_state();
    return s_data.b_SW3Pressed;
  }
}

// we will use the pressed function that runs in the task every 10 seconds
// to set the state of s_data.b_SW1Pressed. For the released we will just return
// the value

bool f15ui_isSW1Released() {
  return !s_data.b_SW1Pressed;
}

bool f15ui_isSW2Released() {
  return !s_data.b_SW2Pressed;
}

bool f15ui_isSW3Released() {
  return !s_data.b_SW3Pressed;
}

// The double pressed functions set the bit to false when the s_data value is set to true after
// the dat value is read. Be aware of this when trying to access double pressed state in multiple
// ESOS task that are trying to read double pressed state at the same time!!!!!

bool f15ui_isSW1DoublePressed() {
  if(s_data.b_SW1doublePressed) {
    s_data.b_SW1doublePressed = false;
    return true;
  }
  return false;
}

bool f15ui_isSW2DoublePressed() {
  if(s_data.b_SW2doublePressed) {
    s_data.b_SW2doublePressed = false;
    return true;
  }
  else {
    return false;
  }
}

bool f15ui_isSW3DoublePressed() {
  if(s_data.b_SW3doublePressed) {
    s_data.b_SW3doublePressed = false;
    return true;
  }
  else {
    return false;
  }
}

// public functions to turn the LED's on

void f15ui_turnLED1ON() {
  __turnLED1_ON();
}

void f15ui_turnLED2ON() {
  __turnLED2_ON();
}

void f15ui_turnLED3ON() {
  __turnLED3_ON();
}

// public funcitons to turn the LED's off

void f15ui_turnLED1OFF() {
  if(s_data.u32_LED1Period) {  // check to see if the LED is flashing
    s_data.u32_LED1Period = 0; // if so turn it off
  }
  __turnLED1_OFF();
}

void f15ui_turnLED2OFF() {
  if(s_data.u32_LED2Period) {
    s_data.u32_LED2Period = 0;
  }
  __turnLED2_OFF();
}

void f15ui_turnLED3OFF() {
  if(s_data.u32_LED3Period) {
    s_data.u32_LED3Period = 0;
  }
  __turnLED3_OFF();
}

// toggles the LEDx

void f15ui_toggleLED1() {
  __toggleLED1();
}

void f15ui_toggleLED2() {
  __toggleLED2();
}

void f15ui_toggleLED3() {
  __toggleLED3();
}

// flash LEDx for u32_period

void f15ui_flashLED1(uint32_t u32_period) {
  s_data.u32_LED1Period = u32_period;
}

void f15ui_flashLED2(uint32_t u32_period) {
  s_data.u32_LED2Period = u32_period;
}

void f15ui_flashLED3(uint32_t u32_period) {
  s_data.u32_LED3Period = u32_period;
}

// turnes xColor LED on

void f15ui_turnRedLEDOn() {
  __turnLED1_ON();
}

void f15ui_turnYellowLEDOn() {
  __turnLED2_ON();
}

void f15ui_turnGreenLEDOn() {
  __turnLED3_ON();
}

// turns xColor LED off

void f15ui_turnRedLEDOff() {
  __turnLED1_OFF();
}

void f15ui_turnYellowLEDOff() {
  __turnLED2_OFF();
}

void f15ui_turnGreenLEDOff() {
  __turnLED3_OFF();
}

// sets Switchx double press period to u32_period

void f15ui_setSW1DPPeriod(uint32_t u32_period) {
  s_data.u32_SW1Period = u32_period;
}

void f15ui_setSW2DPPeriod(uint32_t u32_period) {
  s_data.u32_SW2Period = u32_period;
}

void f15ui_setSW3DPPeriod(uint32_t u32_period) {
  s_data.u32_SW3Period = u32_period;
}

// sets the threshold for RPG speed values

void f15ui_setRPGMedium(uint8_t u16_period) {
  s_data.u16_RPGMedium = u16_period;
}

void f15ui_setRPGFast(uint8_t u16_period) {
  s_data.u16_RPGFast = u16_period;
}

// returns current RPG location value

uint16_t f15ui_getRpgValue_u16() {
  return s_data.u8_cntrROT;
} //return uint16

// returns the velocity of RPG

int16_t f15ui_getRpgVelocity_i16() {
  return s_data.i16_RPGVelocity;
} //returns int16 (>0 for CW, <0 for CCW)

// flowing returns differant states of RPG turning status

bool f15ui_isRpgTurning() {
  if(s_data.b_turningCCW || s_data.b_turningCW) {
    return true;
  }
  return false;
}

bool f15ui_isRpgTurningSlow() {
  return s_data.b_RPGSlow;
}

bool f15ui_isRpgTurningMedium() {
  return s_data.b_RPGMedium;
}

bool f15ui_isRpgTurningFast() {
  return s_data.b_RPGFast;
}

bool f15ui_isRpgTurningCW() {
  return s_data.b_turningCW;
}

bool f15ui_isRpgTurningCCW() {
  return s_data.b_turningCCW;
}

// get first bit of RPGA then shift it to left
// or RPGA with RPGB to give a number between 0-3
// this will be used in the process Rotary Data

/* 			Rocket Propelled Grenades Explained

	3	|   1   | 0	|	2	| 3	|	1	| 0	|	2	|   3

________             ___________             __________________
		|			|			|			|
		|			|			|			|      RPGA
		|			|			|			|
		|___________|			|___________|

________________             ___________             __________
	   	   		|			|			|			|
		    	|			|			|			|    RPGB
			    |			|			|			|
			    |___________|			|___________|

CW    ---->  Sequence goes from 3-1-0-2. 	For example if pervious state is 3 and next state is 1
											delta gets incremented by 1 stating a CW turn

CW    <----  Sequence goes from 3-2-0-1. 	For example if pervious state is 3 and next state is 2
											delta gets decremented by 1 stating a CCW turn

A timer samples the sequence every 15 ms. If no change the timer just skips. If a change it processes
the change and determains delta.

The 15 ms timer seems to increment the turn 2-3 times per our "!!!NORMAL!!!" RPG turn (I cannot predict how a user
will turn the RPG exactly, for instance if a user decides to stop the RPG in the middle of a turn I don't know what would happen). 
I would assume the excate turning would be RPG and user dependent depending on the resolution of the RPG.
This is not an issue until we need to determain revolutions. To determain revolutions on a "!!!NORMAL!!!" RPG turn we can do the following.

startTurn = currentRPGPosition. 
if( each turn == 2 -3 )
take (startTurn +- currentRPGPosition) and shift right by 1
		10 > 1
		11 > 1
store number in another variable currentTurns
when currentTurns = number Of RPG Resolution per revolutions (ours is 12)
the RPG has made a full revolution
*/

#define GET_ROT_STATE() ((RPGA << 1) | RPGB)

// process Rortary Data. This functions updates u8_cntrROT and keeps
// track of current ROT position. the function does return an error code
// that will not update u8_cntrROT. We will ignore this for now but might need it later

uint8_t processRotaryData(uint8_t u8_curr, uint8_t u8_last) {
  int8_t i8_delta = 0;
  switch(u8_curr) {
  case 0:
    if(u8_last == 1) {
      i8_delta = 1;  // if state goes from 10 to 00  CW
    }
    else if (u8_last == 2) {
      i8_delta = -1;  // if state goes from 10 to 00 CCW
    }
    break;
  case 1:
    if(u8_last == 3) {
      i8_delta = 1;  // if state goes from 01 to 11 CW
    }
    else if (u8_last == 0) {
      i8_delta = -1;  // if state goes from 01 to 00 CCW
    }
    break;
  case 3:
    if (u8_last == 2) {
      i8_delta = 1;  // if state goes from 11 to 10 CW
    }
    else if (u8_last == 1) {
      i8_delta = -1;  // if state goes from 11 to 01 CCW
    }
    break;
  case 2:
    if (u8_last == 0) {
      i8_delta = 1;  // if state goes from 10 to 00 CW
    }
    else if (u8_last == 0) {
      i8_delta = -1;  // if state goes from 10 to 00 CCW
    }
    break;
  default: // if we get state error
    break;
  }
  if(i8_delta == 0) { // if we get an error ignore and don't change u8_cntrROT
    return 1;
  }
  (s_data.u8_cntrROT) = (s_data.u8_cntrROT) + i8_delta;
  return 0;
}

// timer to sample RPG state.
// it looks like a user timer works ok for now, if for any reason we need to be
// more accurate we could do a hardware timer, or Dr. Bruce suggested we could do this
// in a user task to save the timer resource

ESOS_USER_TIMER(RPG) {
  s_data.u8_valueROT = GET_ROT_STATE();
  if(s_data.u8_lastvalueROT != s_data.u8_valueROT) {
    s_data.u8_errROT = processRotaryData(s_data.u8_valueROT, s_data.u8_lastvalueROT);
    s_data.u8_lastvalueROT = s_data.u8_valueROT;
  }
}

// runs ever 500 ms and keeps track of the RPG speed
// works by looking at the number of times cntrROT has changed away from
// the previous time.
// calculation is:
//      previous_number_rotations - current_number_rotations = change
//      if change > than threshold for x Speed the RPG is rotating at that speed
//      if the RPG turning at all and is below RPGMedium threshold then it is truning slow

ESOS_USER_TASK(get_rpg_speed) {
  ESOS_TASK_BEGIN();
  static uint8_t u8_tempROTCount = 0;
  while (TRUE) {
    if(u8_tempROTCount != s_data.u8_cntrROT) {
      if(abs(u8_tempROTCount - s_data.u8_cntrROT) >
          s_data.u16_RPGFast) { //more than 8/2 turns in 500ms fast
        s_data.b_RPGFast = true;
      }
      else if(abs(u8_tempROTCount - s_data.u8_cntrROT) >
              s_data.u16_RPGMedium) { //more than 8/2 turns in 500ms fast
        s_data.b_RPGMedium = true;
      }
      else {
        s_data.b_RPGSlow = true;
      }
    }
    else {
      s_data.b_RPGSlow = false;
      s_data.b_RPGMedium = false;
      s_data.b_RPGFast = false;
    }
    u8_tempROTCount = s_data.u8_cntrROT;
    ESOS_TASK_WAIT_TICKS(500);
  }
  ESOS_TASK_END();
} // end */


// UI user task that handles button states, debounces, LED flashing periods, and RPG direction
// all samples depend on the 10ms running period

ESOS_USER_TASK(f15ui_task) {
  ESOS_TASK_BEGIN();
  static uint32_t u32_SW1onReleaseTick;
  static uint32_t u32_SW2onReleaseTick;
  static uint32_t u32_SW3onReleaseTick;
  static bool b_SW1wasPressed = false;
  static bool b_SW1wasRelease = true;
  static bool b_SW2wasPressed = false;
  static bool b_SW2wasRelease = true;
  static bool b_SW3wasPressed = false;
  static bool b_SW3wasRelease = true;
  static uint32_t u32_toggleLED1Tick = 0;
  static uint32_t u32_toggleLED2Tick = 0;
  static uint32_t u32_toggleLED3Tick = 0;
  static uint8_t u8_lastCnt = 0;
  while (TRUE) {
    // check to see if any of the switches are pressed
    // if so update the s_data sturcture
    // Handle SWITCH 1
    if(f15ui_isSW1Pressed()) {
      b_SW1wasPressed = true;
      if(((esos_GetSystemTick() - u32_SW1onReleaseTick) < s_data.u32_SW1Period)
          && (b_SW1wasRelease)) {
        s_data.b_SW1doublePressed = true;
        b_SW1wasRelease = false;
      }
    }
    if(f15ui_isSW1Released() &&b_SW1wasPressed) {
      b_SW1wasPressed = false;
      b_SW1wasRelease = true;
      u32_SW1onReleaseTick = esos_GetSystemTick();
    }
    // Handle SWITCH 2
    if(f15ui_isSW2Pressed()) {
      b_SW2wasPressed = true;
      if(((esos_GetSystemTick() - u32_SW2onReleaseTick) < s_data.u32_SW2Period)
          && (b_SW2wasRelease)) {
        s_data.b_SW2doublePressed = true;
        b_SW2wasRelease = false;
      }
    }
    if(f15ui_isSW2Released() && b_SW2wasPressed) {
      b_SW2wasPressed = false;
      b_SW2wasRelease = true;
      u32_SW2onReleaseTick = esos_GetSystemTick();
    }
    // Handle SWITCH 3
    if(f15ui_isSW3Pressed()) {
      b_SW3wasPressed = true;
      if(((esos_GetSystemTick() - u32_SW3onReleaseTick) < s_data.u32_SW3Period)
          && (b_SW3wasRelease)) {
        s_data.b_SW3doublePressed = true;
        b_SW3wasRelease = false;
      }
    }
    if(f15ui_isSW3Released() && b_SW3wasPressed) {
      b_SW3wasPressed = false;
      b_SW3wasRelease = true;
      u32_SW3onReleaseTick = esos_GetSystemTick();
    }
    //********************************************************************************************
    // FOR LED TOGGLES IF PERIOD IS SET
    if(s_data.u32_LED1Period) {
      if((esos_GetSystemTick() - u32_toggleLED1Tick) > s_data.u32_LED1Period) {
        u32_toggleLED1Tick = esos_GetSystemTick();
        __toggleLED1();
      }
    }
    if(s_data.u32_LED2Period) {
      if((esos_GetSystemTick() - u32_toggleLED2Tick) > s_data.u32_LED2Period) {
        u32_toggleLED2Tick = esos_GetSystemTick();
        __toggleLED2();
      }
    }
    if(s_data.u32_LED3Period) {
      if((esos_GetSystemTick() - u32_toggleLED3Tick) > s_data.u32_LED3Period) {
        u32_toggleLED3Tick = esos_GetSystemTick();
        __toggleLED3();
      }
    }
    //*******************************************************************************************
    // SETS RPG DIRECTIONS
    if(u8_lastCnt != s_data.u8_cntrROT) {
      if(u8_lastCnt > s_data.u8_cntrROT) { //CCW
        s_data.b_turningCCW = true;
      }
      else {                        //CW
        s_data.b_turningCW = true;
      }
      u8_lastCnt = s_data.u8_cntrROT;
    }
    else {
      s_data.b_turningCW = false;
      s_data.b_turningCCW = false;
    }
    //this task will run every ~10 milliseconds to check the states and update values
    ESOS_TASK_WAIT_TICKS(10);
  }
  ESOS_TASK_END();
} // end */

// *********************** UI HARDWARE CONFIG *******************************************************


#define   CONFIG_LED1()   CONFIG_RF4_AS_DIG_OUTPUT()       // LED1
#define   CONFIG_LED2()   CONFIG_RB14_AS_DIG_OUTPUT()      // LED2
#define   CONFIG_LED3()   CONFIG_RB15_AS_DIG_OUTPUT()      // LED3

inline void CONFIG_SW1()  {
  CONFIG_RB13_AS_DIG_INPUT();     //use RB13 for switch input 1
  ENABLE_RB13_PULLUP();           //enable the pullup
}

inline void CONFIG_SW2()  {
  CONFIG_RC15_AS_DIG_INPUT();       // use RC15 for switch input 2
  ENABLE_RC15_PULLUP();             // enable the pullup
}

inline void CONFIG_SW3()  {
  CONFIG_RB12_AS_DIG_INPUT();       // use RC15 for switch input 3
  ENABLE_RB12_PULLUP();             // enable the pullup
}
inline void CONFIG_RPG()  {
  CONFIG_RB8_AS_DIG_INPUT();        // use RB8 for switch input RPGA
  ENABLE_RB8_PULLUP();              // enable the pullup
  CONFIG_RB9_AS_DIG_INPUT();        // use RB9 for switch input RPGB
  ENABLE_RB9_PULLUP();              // enable the pullup
}



// ***********************END UI HARDWARE CONFIG ****************************************************

void config_esos_f15ui() {

  // HW setup
  CONFIG_LED1();      // LED1
  CONFIG_LED2();      // LED2
  CONFIG_LED3();      // LED3

  CONFIG_SW1();       // SW1      
  CONFIG_SW2();       // SW2
  CONFIG_SW3();       // SW3
  CONFIG_RPG();       // RPG
  DELAY_US(1);

  // RPG setup
  s_data.u8_valueROT = 0;             // set RPG location to 0 at startup
  s_data.u8_lastvalueROT = s_data.u8_valueROT;
  s_data.u8_cntrROT = 0;
  // set the LED's to be off
  s_data.b_Led1On = false;
  s_data.b_Led2On = false;
  s_data.b_Led3On = true;
  // set initial SW DP period
  f15ui_setSW1DPPeriod(500);
  f15ui_setSW2DPPeriod(500);
  f15ui_setSW3DPPeriod(500);
  // set initial PRG thresholds
  f15ui_setRPGFast(10);
  f15ui_setRPGMedium(5);
  // register UI tasks
  esos_RegisterTask(f15ui_task);
  esos_RegisterTask(get_rpg_speed);
  // set LED states
  __turnLED1_OFF();
  __turnLED2_OFF();
  __turnLED3_OFF();
  // set switch states
  __setSW1_state();
  __setSW2_state();
  __setSW3_state();
  // start the timer for RPG sampleing
  esos_RegisterTimer( RPG , 10 );
}