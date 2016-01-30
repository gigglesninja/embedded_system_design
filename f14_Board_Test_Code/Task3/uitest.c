//****This is just to ensure that the UI compiles as I write the code***///

#include "../../../pic24lib_all/esos/include/esos.h"
#include "../include/revF15.h"

//  Generally, the user-created semaphores will be defined/allocated here

/************************************************************************
 * User supplied functions
 ************************************************************************
 */

 #define MAX (24)
 #define GET_ROT_STATE() ((RPGA << 1) | RPGB)

uint8_t processRotaryData(uint8_t u8_curr, uint8_t u8_last, uint8_t *pu8_cntr, uint8_t u8_max){

  int8_t i8_delta = 0;
  switch(u8_curr) {
    case 0: 
      if(u8_last == 1) i8_delta = 1;
      else if (u8_last == 2) i8_delta = -1;
      break;
    case 1:
      if(u8_last == 3) i8_delta = 1;
      else if (u8_last == 0) i8_delta = -1;
      break;
    case 3:
      if (u8_last == 2) i8_delta = 1;
      else if (u8_last == 1) i8_delta = -1;
      break;
    case 2:
      if (u8_last == 0) i8_delta = 1;
      else if (u8_last == 0) i8_delta = -1;
      break;
    default:
      break;
  }
  if(i8_delta == 0) 
    return 1;
  (*pu8_cntr) = (*pu8_cntr) + i8_delta;
  return 0;
}

volatile uint8_t u8_valueROT = 0;
volatile uint8_t u8_lastvalueROT = 0;
volatile uint8_t u8_errROT = 0;
volatile uint8_t u8_cntrROT = 0;

ESOS_USER_TIMER(rot){
  u8_valueROT = GET_ROT_STATE();
  if(u8_lastvalueROT != u8_valueROT){
    u8_errROT = processRotaryData(u8_valueROT, u8_lastvalueROT, &u8_cntrROT, MAX);
    u8_lastvalueROT = u8_valueROT;
  }
}

ESOS_USER_TASK(b_test) {
  ESOS_TASK_BEGIN();
  static uint8_t u8_lastCnt = 0;
  while (TRUE) {
      if(u8_lastCnt != u8_cntrROT){
        if(u8_lastCnt > u8_cntrROT){
          ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
          ESOS_TASK_WAIT_ON_SEND_STRING("CCW\n");
          ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        else{
          ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
          ESOS_TASK_WAIT_ON_SEND_STRING("CW\n");
          ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        u8_lastCnt = u8_cntrROT;
      }
    ESOS_TASK_WAIT_TICKS(50);
    }
 // endof while(TRUE)
  ESOS_TASK_END();
} // end upper_case()

/****************************************************
 *  user_init()
 ****************************************************
 */
void user_init(void) {
  u8_valueROT = GET_ROT_STATE();
  u8_lastvalueROT = u8_valueROT;
  CONFIG_RB8_AS_DIG_INPUT();      //use RB8 for RPGA
  ENABLE_RB8_PULLUP();            //enable the pullup
  ENABLE_RB8_CN_INTERRUPT(); 
  CONFIG_RB9_AS_DIG_INPUT();      //use RB9 for RPGB
  ENABLE_RB9_PULLUP();            //enable the pullup
  CONFIG_RB15_AS_DIG_OUTPUT() ;     //LED3
  ENABLE_RB9_CN_INTERRUPT(); 
  DELAY_US(1); 


  LED3 = 0;


  esos_RegisterTask(b_test);
  esos_RegisterTimer( rot, 15 );

} // end user_init()