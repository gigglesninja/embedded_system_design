#ifndef   REV_15H
#define   REV_15H

#include "../../pic24lib_all/esos/include/pic24/esos_pic24.h"

//*****************************LEDS******************************/

//LED1
#define LED1      	(_RF4)
#define CONFIG_LED1()   CONFIG_RF4_AS_DIG_OUTPUT()

//LED2
#define LED2      	(_RB14)
#define CONFIG_LED2()   CONFIG_RB14_AS_DIG_OUTPUT()

//LED3
#define LED3      	(_RB15)
#define CONFIG_LED3()   CONFIG_RB15_AS_DIG_OUTPUT()



//*****************************SWITCHES************************/

//SWITCH 1
#define SW1			(_RB13)
#define SW1_PRESSED()   (SW1==0)        //switch test
#define SW1_RELEASED()  (SW1==1)        //switch test
inline void CONFIG_SW1()  {
  CONFIG_RB13_AS_DIG_INPUT();     	//use RC15 for switch input
  ENABLE_RB13_PULLUP();           	//enable the pullup
  DELAY_US(1); 						// Give the pullup some time to take effect.
}

//SWITCH 2
#define SW2			(_RB12)
#define SW2_PRESSED()   (SW2==0)        //switch test
#define SW2_RELEASED()  (SW2==1)        //switch test
inline void CONFIG_SW2()  {
  CONFIG_RB12_AS_DIG_INPUT();     	//use RC15 for switch input
  ENABLE_RB12_PULLUP();           	//enable the pullup
  DELAY_US(1); 						// Give the pullup some time to take effect.
}

//SWITCH 3
#define SW3			(_RC15)
#define SW3_PRESSED()   (SW3==0)        //switch test
#define SW3_RELEASED()  (SW3==1)        //switch test
inline void CONFIG_SW3()  {
  CONFIG_RC15_AS_DIG_INPUT();     	//use RC15 for switch input
  ENABLE_RC15_PULLUP();           	//enable the pullup
  DELAY_US(1); 						// Give the pullup some time to take effect.
}

//****************Rocket Grenades************************************/

#define ROT_MAX  32            //arbitrary limit



//RPGA
#define RPGA		(_RB8)
#define RPGA_HIGH	(RPGA==1)
#define RPGA_LOW	(RPGA==0)
inline void CONFIG_RPGA()  {
  CONFIG_RB8_AS_DIG_INPUT();     	//use RB8 for switch input
  ENABLE_RB8_PULLUP();           	//enable the pullup
  DELAY_US(1); 						// Give the pullup some time to take effect.
}

#define RPGB		(_RB9)
#define RPGB_HIGH	(RPGB==1)
#define RPGB_LOW	(RPGB==0)
inline void CONFIG_RPGB()  {
  CONFIG_RB9_AS_DIG_INPUT();     	//use RB9 for switch input
  ENABLE_RB9_PULLUP();           	//enable the pullup
  DELAY_US(1); 						// Give the pullup some time to take effect.
}

//***************SPI 1 FOR DAC ***************************************/

#define DAC_ENABLE()  _LATD0 = 1
#define DAC_DISABLE() _LATD0 = 0
#define CONFIG_DAC    (CONFIG_RD0_AS_DIG_OUTPUT())

inline void configSPI1() {
  //spi clock = 40MHz/1*4 = 40MHz/4 = 10MHz
  SPI1CON1 = SEC_PRESCAL_1_1 |     //1:1 secondary prescale
             PRI_PRESCAL_4_1 |     //4:1 primary prescale
             CLK_POL_ACTIVE_HIGH   | //clock active high (CKP = 0)
             SPI_CKE_OFF          | //out changes inactive to active (CKE=0)
             SPI_MODE8_ON        | //8-bit mode
             MASTER_ENABLE_ON;     //master mode
  CONFIG_SDO1_TO_RP(RD4_RP);      //use RB6 for SDO
  CONFIG_RD4_AS_DIG_OUTPUT();   //Ensure that this is a digital output
  CONFIG_SCK1OUT_TO_RP(RB7_RP);   //use RB7 for SCLK
  CONFIG_RB7_AS_DIG_INPUT();   //Ensure that this is a digital input
  CONFIG_SLAVE_ENABLE();       //chip select for MCP41xxx
  SLAVE_DISABLE();             //disable the chip select
  SPI1STATbits.SPIEN = 1;  //enable SPI mode
}

#endif