// James Tate
// IRIG-B reader

#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24.h"
#include "../../include/esos_f15_ui.h"
#include <stdio.h>

bool start = false;
bool go = false;
uint8_t delta; // time between last tick and current tick
uint8_t count; // the current bit that will be set "see irig-b info"
uint32_t currentTime; // current timer tick
uint32_t lastTime; // last timer tick "for the delta"
uint32_t lastPRTime; // last time referance happen
uint8_t bit; // the current bit that will be set\

uint8_t seconds; // holds current seconds
uint8_t minutes; // holds current minutes
uint8_t hours; // holds current hours

void _ISRFAST _CNInterrupt (void) {
  _CNIF = 0;    //clear the change notification interrupt bit
  delta = currentTime - lastTime;
  // check to see if we are at the referance
  if (delta >=7) {
    if ((currentTime - lastPRTime) < 20) {
      count = 1;
      start = TRUE;
    }
    lastPRTime = currentTime;
  }
  // check for a 1
  else if(delta >= 4 && delta <= 6) {
    bit = 1;
    if(start) {
      count++;
    }
  }
  // check for a 0
  else if(delta < 4) {
    bit = 0;
    if(start) {
      count++;
    }
  }
  lastTime = currentTime;
}

void configTimer2(void) {
  T2CON = T2_SOURCE_INT | // internal clock
          T2_PS_1_8 | // 1 to 1 prescale
          T2_OFF    |
          T2_GATE_OFF;
  PR2 = 7499;
  ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T2);
  T2CONbits.TON = 1;
}

ESOS_USER_INTERRUPT(ESOS_IRQ_PIC24_T2) {
  ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T2);
  currentTime++;
  _LATD7 = !_LATD7;
}

void numbertoChar(uint8_t sec) {
  if(sec == 1) {
    printf("1");
  }
  else if(sec == 2) {
    printf("3");
  }
  else if(sec == 4) {
    printf("4");
  }
  else if(sec == 5) {
    printf("5");
  }
  else if(sec == 6) {
    printf("6");
  }
  else if(sec == 7) {
    printf("7");
  }
  else if(sec == 8) {
    printf("8");
  }
  else if(sec == 9) {
    printf("9");
  }
  else if(sec == 10) {
    printf("10");
  }
  else if(sec == 11) {
    printf("11");
  }
  else if(sec == 12) {
    printf("12");
  }
  else if(sec == 13) {
    printf("13");
  }
  else if(sec == 14) {
    printf("14");
  }
  else if(sec == 15) {
    printf("15");
  }
  else if(sec == 16) {
    printf("16");
  }
  else if(sec == 17) {
    printf("17");
  }
  else if(sec == 18) {
    printf("18");
  }
  else if(sec == 19) {
    printf("19");
  }
  else if(sec == 20) {
    printf("20");
  }
  else if(sec == 21) {
    printf("21");
  }
  else if(sec == 22) {
    printf("22");
  }
  else if(sec == 23) {
    printf("23");
  }
  else if(sec == 24) {
    printf("24");
  }
  else {
    printf("0");
  }
}

ESOS_USER_TASK(SECONDS) {
  ESOS_TASK_BEGIN();
  while(TRUE) {
    if(go) {
      printf("hour: ");
      numbertoChar(hours & 0xF0);
      numbertoChar(hours & 0x0F);
      printf(" minutes: ");
      numbertoChar(minutes & 0xF0);
      numbertoChar(minutes & 0x0F);
      printf(" seconds: ");
      numbertoChar(seconds & 0xF0);
      numbertoChar(seconds & 0x0F);
      printf("\n\r");
    }
    ESOS_TASK_WAIT_TICKS(100);
  }
  ESOS_TASK_END();
}


ESOS_USER_TASK(SET) {
  ESOS_TASK_BEGIN();
  while(TRUE) {
    if(count == 3) {
      seconds = 0;
      seconds |= bit << 0;
    }
    else if(count == 5) {
      seconds |= bit << 1;
    }
    else if(count == 7) {
      seconds |= bit << 2;
    }
    else if(count == 9) {
      seconds |= bit << 3;
    }
    else if(count == 13) {
      seconds |= bit << 4;
      LED2 = ~LED2;
    }
    else if(count == 15) {
      seconds |= bit << 5;
    }
    else if(count == 17) {
      seconds |= bit << 6;
    }
    else if(count == 21) {
      minutes = 0;
      minutes |= bit << 0;
    }
    else if(count == 23) {
      minutes |= bit << 1;
    }
    else if(count == 25) {
      minutes |= bit << 2;
    }
    else if(count == 27) {
      minutes |= bit << 3;
    }
    else if(count == 29) {
      minutes |= bit << 4;
    }
    else if(count == 33) {
      minutes |= bit << 5;
    }
    else if(count == 35) {
      minutes |= bit << 6;
    }
    else if(count == 41) {
      hours = 0;
      hours |= bit << 0;
    }
    else if(count == 43) {
      hours |= bit << 1;
    }
    else if(count == 45) {
      hours |= bit << 2;
    }
    else if(count == 47) {
      hours |= bit << 3;
    }
    else if(count == 51) {
      hours |= bit << 4;
    }
    else if(count == 53) {
      hours |= bit << 5;
    }
    ESOS_TASK_WAIT_TICKS(1);
  }
  ESOS_TASK_END();
}

ESOS_USER_TASK(PR2_TIME)  {
  ESOS_TASK_BEGIN();
  while (TRUE) {
    if(f15ui_isSW1Pressed()) {
      PR2 = PR2++;
    }
    if(f15ui_isSW2Pressed()) {
      PR2 = PR2--;
    }
    ESOS_TASK_WAIT_TICKS(250);
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
    if(choice == 0x0D) {
      printf("Hello!!!\r\n");
      printf("I was built by James Tate\r\n");
      printf("My build logs:\r");
      printf(HELLO_MSG);
      printf("\r\n");
    }
    else {
      printf("no");
    }
  }
  ESOS_TASK_END();
}

void user_init(void) {
  CONFIG_RD7_AS_DIG_OUTPUT();
  _LATD7 = 1;
  delta = 0; // time between last tick and current tick
  count = 0; // the current bit that will be set "see irig-b info"
  currentTime = 0; // current timer tick
  lastTime = 0; // last timer tick "for the delta"
  bit = 0; // the current bit that will be set
  seconds = 0; // holds current seconds
  minutes = 0; // holds current minutes
  hours = 0; // holds current hours
  lastPRTime = 0;
  LED1=0;
  _CNIF = 0;         //Clear the interrupt flag
  _CNIP = 1;         //Choose a priority
  _CNIE = 1;         //enable the Change Notification general interrupt
  config_esos_f15ui();
  CONFIG_RD2_AS_DIG_INPUT();       // use RC15 for switch input 2
  ENABLE_RD2_PULLUP();             // enable the pullup
  ENABLE_RD2_CN_INTERRUPT();
  configTimer2(); // configure timer 2
  ESOS_REGISTER_PIC24_USER_INTERRUPT( ESOS_IRQ_PIC24_T2, ESOS_USER_IRQ_LEVEL2,
                                      _T2Interrupt);
  ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T2);
  esos_RegisterTask(RX);
  esos_RegisterTask(PR2_TIME);
  esos_RegisterTask(SET);
  esos_RegisterTask(SECONDS);
  f15ui_flashLED3(500);
} // end user_init()