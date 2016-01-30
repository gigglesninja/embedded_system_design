#ifndef   REV_15H
#define   REV_15H

#include "../../pic24lib_all/esos/include/pic24/esos_pic24.h"

//*****************************LEDS******************************/

//LED1
#define LED1      	_LATF4
//LED2
#define LED2      	_LATB14
//LED3
#define LED3      	_LATB15

//*****************************SWITCHES************************/

//SWITCH 1
#define SW1			(_RB13)
#define SW1_PRESSED()   (SW1==0)        //switch test
#define SW1_RELEASED()  (SW1==1)        //switch test
//SWITCH 2
#define SW2			(_RB12)
#define SW2_PRESSED()   (SW2==0)        //switch test
#define SW2_RELEASED()  (SW2==1)        //switch test
//SWITCH 3
#define SW3			(_RC15)
#define SW3_PRESSED()   (SW3==0)        //switch test
#define SW3_RELEASED()  (SW3==1)        //switch test

//****************Rocket Grenades************************************/

//RPGA
#define RPGA		(_RB8)  //rpga
#define RPGB		(_RB9)  //rpgb

//*******************POT****************************************/

#define CONFIG_POT()	CONFIG_RB2_AS_ANALOG() 	// pic23_ports_config.h line 2672

#define CONFIG_TEMP() 	CONFIG_RB3_AS_ANALOG()  // configuration for the temp sensor

//*****************LCD******************************************/

// data and operation pins mapped to the MCU
# define RS_HIGH()        (_LATC12 = 1)
# define RS_LOW()         (_LATC12 = 0)
# define CONFIG_RS()      CONFIG_RC12_AS_DIG_OUTPUT()

# define RW_HIGH()        (_LATD11 = 1)
# define RW_LOW()         (_LATD11 = 0)
# define CONFIG_RW()      CONFIG_RD11_AS_DIG_OUTPUT()

# define E_HIGH()         (_LATD10 = 1)
# define E_LOW()          (_LATD10 = 0)
# define CONFIG_E()       CONFIG_RD10_AS_DIG_OUTPUT()

# define LCD0O          	(_LATE0)
# define LCD1O          	(_LATE1)
# define LCD2O          	(_LATE2)
# define LCD3O          	(_LATE3)
# define LCD4O          	(_LATE4)
# define LCD5O          	(_LATE5)
# define LCD6O          	(_LATE6)
# define LCD7O          	(_LATE7)
# define BUSY_FLAG          (_RE7)

# define CONFIG_LCD0_AS_INPUT() CONFIG_RE0_AS_DIG_INPUT()
# define CONFIG_LCD1_AS_INPUT() CONFIG_RE1_AS_DIG_INPUT()
# define CONFIG_LCD2_AS_INPUT() CONFIG_RE2_AS_DIG_INPUT()
# define CONFIG_LCD3_AS_INPUT() CONFIG_RE3_AS_DIG_INPUT()
# define CONFIG_LCD4_AS_INPUT() CONFIG_RE4_AS_DIG_INPUT()
# define CONFIG_LCD5_AS_INPUT() CONFIG_RE5_AS_DIG_INPUT()
# define CONFIG_LCD6_AS_INPUT() CONFIG_RE6_AS_DIG_INPUT()
# define CONFIG_LCD7_AS_INPUT() CONFIG_RE7_AS_DIG_INPUT()

# define CONFIG_LCD0_AS_OUTPUT() CONFIG_RE0_AS_DIG_OUTPUT()
# define CONFIG_LCD1_AS_OUTPUT() CONFIG_RE1_AS_DIG_OUTPUT()
# define CONFIG_LCD2_AS_OUTPUT() CONFIG_RE2_AS_DIG_OUTPUT()
# define CONFIG_LCD3_AS_OUTPUT() CONFIG_RE3_AS_DIG_OUTPUT()
# define CONFIG_LCD4_AS_OUTPUT() CONFIG_RE4_AS_DIG_OUTPUT()
# define CONFIG_LCD5_AS_OUTPUT() CONFIG_RE5_AS_DIG_OUTPUT()
# define CONFIG_LCD6_AS_OUTPUT() CONFIG_RE6_AS_DIG_OUTPUT()
# define CONFIG_LCD7_AS_OUTPUT() CONFIG_RE7_AS_DIG_OUTPUT()

#endif