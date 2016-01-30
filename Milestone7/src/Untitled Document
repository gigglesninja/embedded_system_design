#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24_i2c.h"
#include "../../include/esos_f15_ui.h"
#include "../../include/esos_lcd_menu.h"


// DEFINEs go here
#define DS1631ADDR 0x90   //DS1631 address with all pins tied low
#define ACCESS_CONFIG 0xAC
#define CONFIG_COMMAND 0x0C //continuous conversion, 12-bit mode
#define START_CONVERT 0x51
#define READ_TEMP 0xAA

#define TRI      (0x01) // triangle wave
#define SINE     (0x02) // sine wave
#define SQUARE   (0x03) // square wave

// the menu items
#define WAVEFORM (1)
#define FREQ     (2)
#define AMPLTD   (3)
#define DUTY     (4)
#define LEDSET   (6)

#define   CONFIG_LED1()   CONFIG_RB15_AS_DIG_OUTPUT()
#define   LED1            _LATB15

// PROTOTYPEs go here

// GLOBALs go here
//  Generally, the user-created semaphores will be defined/allocated here
char psz_CRNL[3]= {0x0D, 0x0A, 0};
char psz_prompt[] = "Temp is  ";
char psz_done[9]= {' ','D','O','N','E','!',0x0D, 0x0A, 0};
int16_t i16_temp;
UINT16 U16_raw;

ESOS_SEMAPHORE(sem_dataReady);
ESOS_SEMAPHORE(sem_dataPrinted);
ESOS_SEMAPHORE(sem_ds1631Ready);

/************************************************************************
 * User supplied functions
 ************************************************************************
 */

/*
 * An ESOS software timer callback function strobe the heartbeat LED.
 *
 * Toggles LED1 everytime the callback is called.  Exact period is
 * determined by application when this timer callback function is
 * registered with ESOS.  See \ref esos_RegisterTimer
 * Application can change timer period on-the-fly with \ref esos_ChangeTimerPeriod
 *
 * \note Since this heartbeat is performed in an ESOS software
 * timer callabck, a flashing LED indicates that the ESOS system
 * tick ISR is being called properly.  If the LED quits flashing,
 * then the ESOS system tick has ceased working.  This probably indicates
 * some catastrophic failure of the system.  However, the cause could
 * be poorly-behaved user code that is manipulating the hardware registers
 * with the timer or interrupt enables directly.  ESOS provides functions
 * to change state of interrupts and user code should never modify the
 * hardware used by ESOS to implement the system tick.
 * \hideinitializer
 */

// user-created timer callback
ESOS_USER_TIMER( swTimerLED ) {
  LED1 = !LED1;
} //endof swTimerLED

/*
 * user task to setup DS1631 for continuous temperature
 * conversion.  Will signal when DS1631 is ready to be
 * used.
 */

ESOS_USER_TASK(start_ds1631) {
  ESOS_TASK_BEGIN();
  ESOS_TASK_WAIT_TICKS(500);
  ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
  ESOS_TASK_WAIT_ON_WRITE1I2C1(DS1631ADDR, 0x02);
  ESOS_TASK_WAIT_ON_WRITE2I2C1(DS1631ADDR, ACCESS_CONFIG, CONFIG_COMMAND);
  ESOS_TASK_WAIT_ON_WRITE1I2C1(DS1631ADDR, START_CONVERT);
  ESOS_TASK_SIGNAL_AVAILABLE_I2C();
  ESOS_TASK_WAIT_TICKS(500);
  ESOS_SIGNAL_SEMAPHORE(sem_ds1631Ready, 1);
  ESOS_TASK_END();
} //end task start_ds1631

/*
 * user task to read DS1631 every 3/4 second
 * will signal when data has been successfully read
 */
ESOS_USER_TASK(read_ds1631) {
  static uint8_t u8_lo, u8_hi;

  ESOS_TASK_BEGIN();
  ESOS_TASK_WAIT_SEMAPHORE(sem_ds1631Ready, 1);
  while (TRUE) {
    ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
    ESOS_TASK_WAIT_ON_WRITE1I2C1(DS1631ADDR, READ_TEMP);
    ESOS_TASK_WAIT_ON_READ2I2C1(DS1631ADDR, u8_hi, u8_lo);
    ESOS_TASK_SIGNAL_AVAILABLE_I2C();
    U16_raw.u8Msb = u8_hi;
    U16_raw.u8Lsb = u8_lo;
    i16_temp = u8_hi;
    i16_temp = ((i16_temp<<8)|u8_lo);
    ESOS_SIGNAL_SEMAPHORE(sem_dataReady, 1);
    ESOS_TASK_WAIT_TICKS(750);
    ESOS_TASK_WAIT_SEMAPHORE(sem_dataPrinted, 1);
  } //end while()
  ESOS_TASK_END();
} //end task read_ds1631

/*
 * User task to display temperature results from DS1631.
 * Used printf and floating point for convenience.  A
 * production application would likely create custom functions
 * to avoid including these huge libraries.
 */
ESOS_USER_TASK(update) {
  float   f_tempC, f_tempF;

  ESOS_TASK_BEGIN();
  while (TRUE) {
    ESOS_TASK_WAIT_SEMAPHORE(sem_dataReady, 1);
    f_tempC = (float) i16_temp;  //convert to floating point
    f_tempC = f_tempC/256;  //divide by precision
    f_tempF = f_tempC*9/5 + 32;
    printf("Temp is: 0x%0X, %4.4f (C), %4.4f (F)\n\r", i16_temp, (double) f_tempC, (double) f_tempF);
    ESOS_SIGNAL_SEMAPHORE(sem_dataPrinted, 1);
  } // end while(TRUE)
  ESOS_TASK_END();
} // end update()

/****************************************************
 *  user_init()
 ****************************************************
 */
void user_init(void) {
  // configure our hardware to support to support our application
  esos_init_menu();
  CONFIG_LED1();
  esos_pic24_configI2C1(400);            //configure I2C for 400 KHz
  ESOS_INIT_SEMAPHORE(sem_ds1631Ready, 0);
  ESOS_INIT_SEMAPHORE(sem_dataReady, 0);
  ESOS_INIT_SEMAPHORE(sem_dataPrinted, 0);

  // user_init() should register at least one user task
  esos_RegisterTask(start_ds1631);
  esos_RegisterTask(read_ds1631);
  esos_RegisterTask(update);

  // register our callback function with ESOS to create a software timer
  esos_RegisterTimer( swTimerLED, 250);
    esos_create_menu(7);
  // setup the menu
  // see the .h file for details
  esos_insert_menu_title(WAVEFORM, SET, SV3C, "wvform");
  esos_insert_menu_title(FREQ, SET, SVDATA, "freq");
  esos_insert_menu_title(AMPLTD, SET, SVDATA, "ampltd");
  esos_insert_menu_title(DUTY, SET, SVDATA, "duty");
  esos_insert_menu_title(5, READ, NONE, "LM60");
  esos_insert_menu_title(LEDSET, SET, SVDATA, "LEDs");
  esos_insert_menu_title(7, STATIC, NONE, "About");
  esos_setSV3C(WAVEFORM, "tri   ", "sine  ", "square ");
  esos_setSVDATA(FREQ, 64, 2047, FALSE, FALSE);
  esos_setSVDATA(AMPLTD, 0, 30, FALSE, FALSE);
  esos_setSVDATA(DUTY, 0, 100, FALSE, FALSE);
  esos_setSVDATA(5, 0, 100, FALSE, FALSE);
  esos_setSVDATA(LEDSET, 0, 1110, TRUE, TRUE);
  esos_setSensorReading(5, ESOS_SENSOR_CH03);
  esos_setStaticInfo(7, "Developers DIGITAL FUNCTION SYNTHESIZER",
                     "James CPE Andrew EE John CPE Wyatt CPE");
  esos_setSVDATAValue(DUTY, 50); // set the default duty cycle
  esos_setSVDATAValue(AMPLTD, 30); // set the default ampl

  // Call the hardware-provided routines to print the
  // HELLO_MSG to the screen.  Must use this call because
  // the ESOS communications subsystems is not yet fully
  // initialized, since this call is in user_init()
  //
  // In general, users should call hardware-specific
  // function like this.
  __esos_unsafe_PutString( HELLO_MSG );

} // end user_init()
