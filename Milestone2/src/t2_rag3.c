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

// INCLUDEs go here  (First include the main esos.h file)
#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24.h"
#include "../../../pic24lib_all/lib/include/pic24_delay.h"
#include "../../include/revF15.h"
#include <stdbool.h>

//macro for the lights
#define GREENON     (0)
#define GREENOFF    (1)
#define RUSH_HOUR_TRAFFIC_FLOW    (30000) //30 seconds
#define REGULAR_TRAFFIC_FLOW      (10000)  //10 seconds

// GLOBALs go here
// logic for saving the state of each LED
bool b_NSRed = ON;
bool b_NSYellow = OFF;
bool b_NSGreen = GREENOFF;
bool b_EWRed = OFF;
bool b_EWYellow = OFF;
bool b_EWGreen = GREENON;
bool b_rushHour = false;
bool b_leftTurn = false;

uint16_t u16_allowTraffic = REGULAR_TRAFFIC_FLOW; //start with regular flow
uint16_t u16_cautionTraffic = 3000; //3 seconds
uint16_t u16_allowLeftTurn = 10000; //10 seconds
uint16_t u16_holdRed = 1000; //1 second delay on both

//  Generally, the user-created semaphores will be defined/allocated here


/*******************************************0*****************************
 * User supplied functions
 ************************************************************************
 */

//if left turn traffic needs to flow, block parent task and run this task for 10 seconds
ESOS_CHILD_TASK(leftTurnTraffic, bool *b_turnLight) {
  ESOS_TASK_BEGIN();
  static uint32_t u32_runTime;
  u32_runTime = esos_GetSystemTick() + u16_allowLeftTurn;
  do {
    *b_turnLight = !*b_turnLight;
    ESOS_TASK_WAIT_TICKS(125); // LED OFF/ON ~.25sc
  }
  while(u32_runTime > esos_GetSystemTick());
  ESOS_TASK_END();
}

//keeps track of the which light should be illuminated
ESOS_USER_TASK(street) {
  ESOS_TASK_BEGIN();
  static ESOS_TASK_HANDLE th_child;
  while (TRUE) {
    //check to see if we should allow left turn traffic
    if(b_leftTurn) {
      ESOS_ALLOCATE_CHILD_TASK(th_child);
      ESOS_TASK_SPAWN_AND_WAIT(th_child, leftTurnTraffic, &b_EWGreen);
    }
    b_EWGreen = GREENON;
    ESOS_TASK_WAIT_TICKS(u16_allowTraffic);
    //caution EW traffic
    b_EWYellow = ON;
    b_EWGreen = GREENOFF;
    ESOS_TASK_WAIT_TICKS(u16_cautionTraffic);
    b_EWYellow = OFF;
    b_EWRed = ON;
    //if rush hour hold red for 1 second
    if(b_rushHour) {
      ESOS_TASK_WAIT_TICKS(u16_holdRed);
    }
    //stop EW traffic
    //allow NS traffic
    b_NSRed= OFF;
    if(b_leftTurn) {
      ESOS_ALLOCATE_CHILD_TASK(th_child);
      ESOS_TASK_SPAWN_AND_WAIT(th_child, leftTurnTraffic, &b_NSGreen);
    }
    b_NSGreen = GREENON;
    ESOS_TASK_WAIT_TICKS(u16_allowTraffic);
    //caution NS traffic
    b_NSGreen = GREENOFF;
    b_NSYellow = ON;
    ESOS_TASK_WAIT_TICKS(u16_cautionTraffic);
    b_NSYellow = OFF;
    b_NSRed = ON;
    //if rush hour hold red for 1 second
    if(b_rushHour) {
      ESOS_TASK_WAIT_TICKS(u16_holdRed);
    }
    //allow EW traffic
    b_EWRed = OFF;
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end street

//task uses SW3 to output the street light direction
//SW1 is used to start rush hour traffic
ESOS_USER_TASK(lightView) {
  static bool b_changeStreet = false;
  static bool b_startRushHour = false;
  static bool b_enableLeftTurn = false;
  ESOS_TASK_BEGIN();
  while (TRUE) {
    //Check and sample SW1 for rush hour
    if(SW1_PRESSED()) {
      DELAY_MS(DEBOUNCE_DLY);
      b_startRushHour = !SW1;
    }
    //Check and sample SW1 for left turn traffic
    if(SW2_PRESSED()) {
      DELAY_MS(DEBOUNCE_DLY);
      b_enableLeftTurn = !SW2;
    }
    //Check and sample SW3 for choosing street view
    if(SW3_PRESSED()) {
      DELAY_MS(DEBOUNCE_DLY);
      b_changeStreet = !SW3;
    }
    //if switch 3 is pressed
    if(b_changeStreet) {
      LED1 = b_EWRed;
      LED2 = b_EWYellow;
      LED3 = b_EWGreen;
      b_changeStreet = false;
    }
    else {
      LED1 = b_NSRed;
      LED2 = b_NSYellow;
      LED3 = b_NSGreen;
    }
    //lets see if we need to do rush hour timing
    //timing will start after street task has been given the opportunity to run
    if(b_startRushHour) {
      b_rushHour = true;
      u16_allowTraffic =
        RUSH_HOUR_TRAFFIC_FLOW; //This will switch to the rush hour timing
      //when the light task is executed next. This will stop any collisions or suddon changes
      //between the two states.
      b_startRushHour = false;
    }
    else {
      b_rushHour = false;
      u16_allowTraffic = REGULAR_TRAFFIC_FLOW;
    }
    //check to see if we need to allow traffic to turn left
    if(b_enableLeftTurn) {
      b_leftTurn = true;
      b_enableLeftTurn = false;
    }
    else {
      b_leftTurn = false;
    }
    ESOS_TASK_YIELD();
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end lightView task

/****************************************************
 *  user_init()
 ****************************************************
 */
void user_init(void) {
  // configure our hardware to support to support our application
  CONFIG_LED1();
  CONFIG_LED2();
  CONFIG_LED3();
  CONFIG_SW1();
  CONFIG_SW2();
  CONFIG_SW3();
  // user_init() should register at least one user task
  esos_RegisterTask(street);
  esos_RegisterTask(lightView);
} // end user_init()
