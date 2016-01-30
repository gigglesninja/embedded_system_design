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
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24.h"
#include "../../../pic24lib_all/lib/include/pic24_delay.h"
#include "../../revF15.h"
#include <stdbool.h>


// GLOBALs go here


//  Generally, the user-created semaphores will be defined/allocated here


/************************************************************************
 * User supplied functions
 ************************************************************************
 */

ESOS_USER_TASK(FunctionSynthesizer) {
  ESOS_TASK_BEGIN();
  static uint8_t u8_choice;
  static uint8_t u16_setting;
  while (TRUE) {
    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("1. Set wvform");
    ESOS_TASK_WAIT_ON_SEND_STRING("2. Set freq");
    ESOS_TASK_WAIT_ON_SEND_STRING("3. Set ampltd");
    ESOS_TASK_WAIT_ON_SEND_STRING("4. Set duty");
    ESOS_TASK_WAIT_ON_SEND_STRING("5. Read LM60");
    ESOS_TASK_WAIT_ON_SEND_STRING("6. Set LEDs");
    ESOS_TASK_WAIT_ON_SEND_STRING("7. About...");
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    ESOS_TASK_WAIT_ON_GET_UINT8(u8_choice);
    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    if(u8_choice != '5' && (u8_choice != '7')){
      if(u8_choice == '1'){
        ESOS_TASK_WAIT_ON_SEND_STRING("Choose waveform: (1) Triangle, (2) Sine, (3) Square");
      }
      else if(u8_choice == '2'){
        ESOS_TASK_WAIT_ON_SEND_STRING("Frequency range: 64-2047");
      }
      else if(u8_choice == '3'){
        ESOS_TASK_WAIT_ON_SEND_STRING("Amplitude range: 0-3.3v");
      }
      else if(u8_choice == '4'){
        ESOS_TASK_WAIT_ON_SEND_STRING("Duty cycle range: 0-100");
      }
      else if(u8_choice == '6'){
        ESOS_TASK_WAIT_ON_SEND_STRING("Set number: 0-7");
      }
      ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
      ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
      ESOS_TASK_WAIT_ON_GET_UINT16(u16_setting);
    }
    else{
      if(u8_choice == '5'){
        //Read LM60
        NOP();
      }
      else if(u8_choice == '7'){
        ESOS_TASK_WAIT_ON_SEND_STRING("Created by Andrew Hegman and friends. James Tate had the least involvement, and is by far the weakest member of the team.");

      }
    }



  } // endof while(TRUE)
  ESOS_TASK_END();
} // end upper_case()

/****************************************************
 *  user_init()
 ****************************************************
 */
void user_init(void) {

  // configure our hardware to support to support our application


  // user_init() should register at least one user task
  esos_RegisterTask(FunctionSynthesizer);

} // end user_init()
