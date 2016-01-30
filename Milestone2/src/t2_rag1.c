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


// INCLUDEs go here  (First include the main esos.h file)
#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24.h"
#include "../../../pic24lib_all/lib/include/pic24_delay.h"
#include "../../include/revF15.h"
#include <stdbool.h>

//macro for the lights
#define GREENON     (0)
#define GREENOFF    (1)

// GLOBALs go here
// logic for saving the state of each LED
bool b_NSRed = ON;
bool b_NSYellow = OFF;
bool b_NSGreen = GREENOFF;
bool b_EWRed = OFF;
bool b_EWYellow = OFF;
bool b_EWGreen = GREENON;

//  Generally, the user-created semaphores will be defined/allocated here

/************************************************************************
 * User supplied functions
 ************************************************************************
 */

//keeps track of the which light should be illuminated
ESOS_USER_TASK(street) {
  static uint16_t u16_allowTraffic = 10000;
  static uint16_t u16_cautionTraffic = 3000;
  ESOS_TASK_BEGIN();
  while (TRUE) {
    //we set all the values initially so we just start waiting
    ESOS_TASK_WAIT_TICKS(u16_allowTraffic);
    //caution EW traffic
    b_EWYellow = ON;
    b_EWGreen = GREENOFF;
    ESOS_TASK_WAIT_TICKS(u16_cautionTraffic);
    //stop EW traffic
    //allow NS traffic
    b_NSRed= OFF;
    b_EWYellow = OFF;
    b_NSGreen = GREENON;
    b_EWRed = ON;
    ESOS_TASK_WAIT_TICKS(u16_allowTraffic);
    //caution NS traffic
    b_NSGreen = GREENOFF;
    b_NSYellow = ON;
    ESOS_TASK_WAIT_TICKS(u16_cautionTraffic);
    //allow EW traffic
    b_EWRed = OFF;
    b_NSYellow = OFF;
    b_NSRed = ON;
    b_EWGreen = GREENON;
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end street task

//task uses SW3 to output the street light direction
ESOS_USER_TASK(lightView) {
  static bool b_changeStreet = false;
  ESOS_TASK_BEGIN();
  while (TRUE) {
    //check to see if switch is pressed to view street output
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
  CONFIG_SW3();
  // user_init() should register at least one user task
  esos_RegisterTask(street);
  esos_RegisterTask(lightView);
} // end user_init()