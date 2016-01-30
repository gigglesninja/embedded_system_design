// TEAM APPLE!

// FINAL TASK!!! THANK GOD THIS IS OVER!!!!

// INCLUDEs go here  (First include the main esos.h file)
//      After that, the user can include what they need
//#include "revF15.h"
#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24.h"
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24_rs232.h"
#include <stdio.h>

// TASK for the setting team
ESOS_USER_TASK(TX)  {
  ESOS_TASK_BEGIN();
  while (TRUE) {
  	printf("test");
  	ESOS_TASK_WAIT_TICKS(1000);
  }
  ESOS_TASK_END();
} 

ESOS_USER_TASK(RX)  {
  ESOS_TASK_BEGIN();
  char choice;
  while (TRUE) {
  	ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    ESOS_TASK_WAIT_ON_GET_UINT8(choice);
    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
    if(choice == 0x0D){
    	printf(HELLO_MSG);
    }
    else
    	printf("no");
  }
  ESOS_TASK_END();
} 

void user_init(void) {
  __esos_unsafe_PutString( HELLO_MSG );
 esos_RegisterTask(RX); // UPDATES THE TEMP
} // end user_init()