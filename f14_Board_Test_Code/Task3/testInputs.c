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
// by a *user task* that runs simultaneously with the echo task above.-//
//  .. note::
//
//     Demonstrates multitasking in ESOS



// INCLUDEs go here  (First include the main esos.h file)
//      After that, the user can include what they need
//#include "revF15.h"
#include "../../pic24lib_all/esos/include/esos.h"
#include "../../pic24lib_all/esos/include/pic24/esos_pic24.h"
#include "../../pic24lib_all/lib/include/pic24_delay.h"
#include "../revF15.h"
#include "../include/esos_f15ui.h"
#include <stdbool.h>

//  Generally, the user-created semaphores will be defined/allocated here


/************************************************************************
 * User supplied functions
 ************************************************************************
 */

//keeps track of the which light should be illuminated
ESOS_USER_TASK(testInput) {
  ESOS_TASK_BEGIN();
  while (TRUE) {
    if(SW1_PRESSED()) {
      LED1 = ON;
    }
    if(SW1_PRESSED()) {
      LED2=!LED2;
    }
    if(SW1_PRESSED()) {
      LED3=!LED3;
    }
    ESOS_TASK_YIELD();
  } // endof while(TRUE)
  ESOS_TASK_END();
}

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
  esos_RegisterTask(testInput);
} // end user_init()