// TEAM APPLE!

#ifndef   F15_UI_H
#define   F15_UI_H

// I N C L U D E S **********************************************************/

#include "../../pic24lib_all/esos/include/esos.h"
#include "revF15.h"
#include <stdbool.h>

// /* S T R U C T U R E S ******************************************************/
// //this structure contains the vairaibles that will be returned when a queary
// //is sent to the UI
typedef struct _st_esos_f15UiData {
  bool b_SW1Pressed;
  bool b_SW2Pressed;
  bool b_SW3Pressed;
  bool b_Led1On;
  bool b_Led2On;
  bool b_Led3On;
  bool b_pnrSW1;
  bool b_pnrSW2;
  bool b_pnrSW3;
  bool b_SW1doublePressed;
  bool b_SW2doublePressed;
  bool b_SW3doublePressed;
  bool b_turningCW;
  bool b_turningCCW;
  bool b_RPGFast;
  bool b_RPGMedium;
  bool b_RPGSlow;
  uint8_t u8_valueROT;
  uint8_t u8_lastvalueROT;
  uint8_t u8_errROT;
  uint8_t u8_cntrROT;
  uint8_t u16_RPGMedium;
  uint8_t u16_RPGFast;
  uint32_t u32_currentRPGTick;
  uint32_t u32_previousRPGTick;
  uint32_t u32_LED1Period ;
  uint32_t u32_LED2Period;
  uint32_t u32_LED3Period;
  uint32_t u32_SW1Period;
  uint32_t u32_SW2Period;
  uint32_t u32_SW3Period;
  int16_t i16_RPGVelocity;
} st_esos_f15UiData;

// // // M A C R O S ************************************************************/


// // //------------B L O C K I N G  C A L L S ----------------------------------

// blocks until the said switch is pressed
#define ESOS_TASK_F15UI_WAIT_UNTIL_SW1_PRESSED()	          ESOS_TASK_WAIT_UNTIL(f15ui_isSW1Pressed())
#define ESOS_TASK_F15UI_WAIT_UNTIL_SW2_PRESSED()              ESOS_TASK_WAIT_UNTIL(f15ui_isSW2Pressed())
#define ESOS_TASK_F15UI_WAIT_UNTIL_SW3_PRESSED()              ESOS_TASK_WAIT_UNTIL(f15ui_isSW3Pressed())

// blocks until the said switch is released
#define ESOS_TASK_F15UI_WAIT_UNTIL_SW1_RELEASED()             ESOS_TASK_WAIT_UNTIL(f15ui_isSW1Released())
#define ESOS_TASK_F15UI_WAIT_UNTIL_SW2_RELEASED()             ESOS_TASK_WAIT_UNTIL(f15ui_isSW2Released())
#define ESOS_TASK_F15UI_WAIT_UNTIL_SW3_RELEASED()             ESOS_TASK_WAIT_UNTIL(f15ui_isSW3Released())

// blocks until the said switch is pressed and released
#define ESOS_TASK_F15UI_WAIT_UNTIL_SW1_PRESSED_AND_RELEASED() \
do{                                                           \
  ESOS_TASK_F15UI_WAIT_UNTIL_SW1_PRESSED();                   \
  ESOS_TASK_F15UI_WAIT_UNTIL_SW1_RELEASED();                  \
}while(0);

#define ESOS_TASK_F15UI_WAIT_UNTIL_SW2_PRESSED_AND_RELEASED() \
do{                                                           \
  ESOS_TASK_F15UI_WAIT_UNTIL_SW2_PRESSED();                   \
  ESOS_TASK_F15UI_WAIT_UNTIL_SW2_RELEASED();                  \
}while(0);
#define ESOS_TASK_F15UI_WAIT_UNTIL_SW3_PRESSED_AND_RELEASED() \
do{                                                           \
  ESOS_TASK_F15UI_WAIT_UNTIL_SW3_PRESSED();                   \
  ESOS_TASK_F15UI_WAIT_UNTIL_SW3_RELEASED();                  \
}while(0);

// blocks until the said switch is double pressed
#define ESOS_TASK_F15UI_WAIT_UNTIL_SW1_DOUBLE_PRESSED()       ESOS_TASK_WAIT_UNTIL(f15ui_isSW1DoublePressed())
#define ESOS_TASK_F15UI_WAIT_UNTIL_SW2_DOUBLE_PRESSED()       ESOS_TASK_WAIT_UNTIL(f15ui_isSW2DoublePressed())
#define ESOS_TASK_F15UI_WAIT_UNTIL_SW3_DOUBLE_PRESSED()       ESOS_TASK_WAIT_UNTIL(f15ui_isSW3DoublePressed())

// // RPG BLOCKING CALLS
#define ESOS_TASK_F15UI_WAIT_UNTIL_RPG_UNTIL_TURNS()          ESOS_TASK_WAIT_UNTIL(f15ui_isRpgTurning())
#define ESOS_TASK_F15UI_WAIT_UNTIL_RPG_UNTIL_STOPS()          ESOS_TASK_WAIT_UNTIL(!f15ui_isRpgTurning())
#define ESOS_TASK_F15UI_WAIT_UNTIL_RPG_UNTIL_TURNS_CW()       ESOS_TASK_WAIT_UNTIL(f15ui_isRpgTurningCW())
#define ESOS_TASK_F15UI_WAIT_UNTIL_RPG_UNTIL_TURNS_CCW()      ESOS_TASK_WAIT_UNTIL(f15ui_isRpgTurningCCW())
#define ESOS_TASK_F15UI_WAIT_UNTIL_RPG_TURNS_MEDIUM()         ESOS_TASK_WAIT_UNTIL(f15ui_isRpgTurningMedium())
#define ESOS_TASK_F15UI_WAIT_UNTIL_RPG_TURNS_MEDIUM_CW()      ESOS_TASK_WAIT_UNTIL(f15ui_isRpgTurningMedium() && f15ui_isRpgTurningCW())
#define ESOS_TASK_F15UI_WAIT_UNTIL_RPG_TURNS_MEDIUM_CCW()     ESOS_TASK_WAIT_UNTIL(f15ui_isRpgTurningMedium() && f15ui_isRpgTurningCCW())
#define ESOS_TASK_F15UI_WAIT_UNTIL_RPG_TURNS_FAST()           ESOS_TASK_WAIT_UNTIL(f15ui_isRpgTurningFast())
#define ESOS_TASK_F15UI_WAIT_UNTIL_RPG_TURNS_FAST_CW()        ESOS_TASK_WAIT_UNTIL(f15ui_isRpgTurningFast() && f15ui_isRpgTurningCW())
#define ESOS_TASK_F15UI_WAIT_UNTIL_RPG_TURNS_FAST_CCW()       ESOS_TASK_WAIT_UNTIL(f15ui_isRpgTurningFast() && f15ui_isRpgTurningCCW())
#define ESOS_TASK_F15UI_WAIT_UNTIL_RPG_MAKES_REV(y)           ESOS_TASK_WAIT_UNTIL(_RPGRev(y))
#define ESOS_TASK_F15UI_WAIT_UNTIL_RPG_MAKES_CW_REV(y)        ESOS_TASK_WAIT_UNTIL(_RPGRevCW(y))
#define ESOS_TASK_F15UI_WAIT_UNTIL_RPG_MAKES_CCW_REV(y)       ESOS_TASK_WAIT_UNTIL(_RPGRevCCW(y))


// // P U B L I C    P R O T O T Y P E S *************************************/

bool _RPGRev(uint8_t u8_rev);  // returns true when RPG makes i8_rev turns
bool _RPGRevCW(uint8_t u8_rev); // returns true when RPG makes i8_rev turns CW
bool _RPGRevCCW(uint8_t u8_rev); // returns true when RPG makes i8_rev turns CCW

bool f15ui_isSW1Pressed(); // returns true when SW1 pressed
bool f15ui_isSW2Pressed(); // returns true when SW2 pressed
bool f15ui_isSW3Pressed(); // returns true when SW3 pressed
bool f15ui_isSW1Released(); // returns true when SW1 released
bool f15ui_isSW2Released(); // returns true when SW2 released
bool f15ui_isSW3Released(); // returns true when SW3 released
bool f15ui_isSW1DoublePressed(); // returns true when SW1 is DP ;; clears the bit
bool f15ui_isSW2DoublePressed(); // returns true when SW2 is DP ;; clears the bit
bool f15ui_isSW3DoublePressed(); // returns true when SW3 is DP ;; clears the bit

void f15ui_turnLED1ON(); // turns LED1 on
void f15ui_turnLED2ON(); // turns LED2 on
void f15ui_turnLED3ON(); // turns LED3 on
void f15ui_turnLED1OFF(); // turns LED1 off
void f15ui_turnLED2OFF(); // turns LED2 off
void f15ui_turnLED3OFF(); // turns LED3 off
void f15ui_toggleLED1(); // toggles LED1
void f15ui_toggleLED2(); // toggles LED2
void f15ui_toggleLED3(); // toggles LED3
void f15ui_flashLED1(uint32_t u32_period); // flash LED1 u32_period
void f15ui_flashLED2(uint32_t u32_period); // flash LED2 u32_period
void f15ui_flashLED3(uint32_t u32_period); // flash LED3 u32_period
void f15ui_turnRedLEDOn(); // turns RED LED on
void f15ui_turnYellowLEDOn(); // turns yellow LED on
void f15ui_turnGreenLEDOn(); // turns green LED on
void f15ui_turnRedLEDOff(); // turns red led off
void f15ui_turnYellowLEDOff(); // turns yellow led off
void f15ui_turnGreenLEDOff(); // turns green led off

void f15ui_setSW1DPPeriod(uint32_t u32_period); // sets SW1 double press period to u32_period
void f15ui_setSW2DPPeriod(uint32_t u32_period); // sets SW2 double press period to u32_period
void f15ui_setSW3DPPeriod(uint32_t u32_period); // sets SW3 double press period to u32_period
void f15ui_setRPGMedium(uint8_t u16_period); // sets RPG medium turn period
void f15ui_setRPGFast(uint8_t u16_period); // sets RPG fast turn period

uint16_t f15ui_getRpgValue_u16(void); //return a uint16 of the current RPG turn value
bool f15ui_isRpgTurning(); // returns true if RPG is turning
bool f15ui_isRpgTurningMedium(); //returns true if RPG is turning medium
bool f15ui_isRpgTurningFast(); //returns true if RPG is turning fast
bool f15ui_isRpgTurningCW(); //returns true if RPG is turning Clock wise
bool f15ui_isRpgTurningCCW(); //returns true if RPG is turning counter clock wise
int16_t f15ui_getRpgVelocity_i16(); //returns int16 for the rpg velocity

void config_esos_f15ui(void);
#endif