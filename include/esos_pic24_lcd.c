// TEAM APPLE!

#include    "esos_pic24_lcd.h"
#include "stdio.h"

// ******** G L O B A L S ***************

memoryBuffer st_LCDmemoryBuffer; // struct that will hold the memory buffer

// ******************* C O M M A N D  I N F O ********************************************************
//____________________________________________________________________________________________________
//    A nice Manual for commands
//    http://mil.ufl.edu/3744/docs/lcdmanual/commands.html#Cds
//____________________________________________________________________________________________________

// ***************** L C D  2  L I N E  A D D R E S S ************************************************
/* http://courses.ece.msstate.edu/ece4723/datasheets/ST7066U.pdf [page 11]
    Visable Static

    | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08
    | 40 | 41 | 42 | 43 | 44 | 45 | 46 | 47 | 48

*/

// ***************** B U F F E R  I N F O ************************************************************
/*    http://courses.ece.msstate.edu/ece4723/datasheets/ST7066U.pdf [page 10]
      Display data RAM (DDRAM) stores display data represented in 8-bit character codes. Its extended
      capacity is 80 x 8 bits, or 80 characters. The area in display data RAM (DDRAM) that is not used
      for display can be used as general data RAM. See Figure 1 for the relationships between DDRAM
      addresses and positions on the liquid crystal display. The DDRAM address (ADD ) is set in the
      address counter (AC) as hexadecimal.
*/

// *********************** LCD HARDWARE CONFIG *******************************************************

// sets the LCD BUS
#define LCD_OUT_BUS LATE
#define LCD_IN_BUS  PORTE

void set_LCD_out_bus(uint8_t u8_data) {
  LCD_OUT_BUS = u8_data;
}

// configs the pins for the LCD screen
inline void esos_pic24_config_char_lcd(void)  {
  CONFIG_RS();     //RS
  CONFIG_RW();     //RW
  CONFIG_E();      //E
  // setup module to a known state during init
  RS_LOW();
  RW_LOW();
  E_LOW();
}

//  Configure data bus for output
void configBusAsOutLCD(void) {
  RW_LOW();                  //RW=0 to stop LCD from driving pins
  CONFIG_LCD0_AS_OUTPUT();   //D0
  CONFIG_LCD1_AS_OUTPUT();   //D1
  CONFIG_LCD2_AS_OUTPUT();   //D2
  CONFIG_LCD3_AS_OUTPUT();   //D3
  CONFIG_LCD4_AS_OUTPUT();   //D4
  CONFIG_LCD5_AS_OUTPUT();   //D5
  CONFIG_LCD6_AS_OUTPUT();   //D6
  CONFIG_LCD7_AS_OUTPUT();   //D7
}

//  Configure data bus for input
void configBusAsInLCD(void) {
  CONFIG_LCD0_AS_INPUT();   //D0
  CONFIG_LCD1_AS_INPUT();   //D1
  CONFIG_LCD2_AS_INPUT();   //D2
  CONFIG_LCD3_AS_INPUT();   //D3
  CONFIG_LCD4_AS_INPUT();   //D4
  CONFIG_LCD5_AS_INPUT();   //D5
  CONFIG_LCD6_AS_INPUT();   //D6
  CONFIG_LCD7_AS_INPUT();   //D7
  ENABLE_RE0_PULLUP();
  ENABLE_RE1_PULLUP();
  ENABLE_RE2_PULLUP();
  ENABLE_RE3_PULLUP();
  ENABLE_RE4_PULLUP();
  ENABLE_RE5_PULLUP();
  ENABLE_RE6_PULLUP();
  ENABLE_RE7_PULLUP();
  RW_HIGH();                   // R/W = 1, for read
}

//  Pulse the E clock, 1 us delay around edges for setup/hold times
void toggleE(void) {
  DELAY_MS(1);
  E_HIGH();
  DELAY_MS(1);
  E_LOW();
  DELAY_MS(1);
}

// ***********************END LCD HARDWARE CONFIG ****************************************************

void __esos_pic24_lcd_readByte(uint8_t *u8_data, uint8_t u8_getCursor,
                               uint8_t u8_useBusyFlag) {
  uint8_t u8_BusyFlag;
  if (u8_useBusyFlag) {
    RS_LOW();            //RS = 0 to check busy
    configBusAsInLCD();  //set data pins all inputs
    do {
      E_HIGH();
      DELAY_US(1);
      u8_BusyFlag = BUSY_FLAG;
      E_LOW();
      DELAY_US(1);
    }
    while (u8_BusyFlag);
  }
  else {
    DELAY_MS(10); // don't use busy, just delay
  }
  configBusAsOutLCD();
  if (u8_getCursor) {
    RS_HIGH();   // RS=1, data byte
  }
  else {
    RS_LOW();    // RS=0, Get Cursor Position
  }
  E_HIGH();
  DELAY_US(1);
  *u8_data = LCD_IN_BUS;
  E_LOW();
  DELAY_US(1);
}

void __esos_pic24_lcd_writeByte(uint8_t u8_data, uint8_t u8_isData,
                                uint8_t u8_useBusyFlag) {
  uint8_t u8_BusyFlag;
  if (u8_useBusyFlag) {
    RS_LOW();            //RS = 0 to check busy
    configBusAsInLCD();  //set data pins all inputs
    do {
      E_HIGH();
      DELAY_US(1);
      u8_BusyFlag = BUSY_FLAG;
      E_LOW();
      DELAY_US(1);
    }
    while (u8_BusyFlag);
  }
  else {
    DELAY_MS(10); // don't use busy, just delay
  }
  configBusAsOutLCD();
  if (u8_isData) {
    RS_HIGH();   // RS=1, data byte
  }
  else {
    RS_LOW();             // RS=0, command byte
  }
  DELAY_MS(10);
  set_LCD_out_bus(u8_data);  // set out bus
  toggleE();
}

// ******** C H I L D  T A S K ******************

/*u8_data is the data to write or the location for the stored read data, u8_isData determines
whether the u8_data is LCD module data (TRUE) or a LCD module command (FALSE),
and u8_data determines whether the routines will poll the LCD module busy flag (TRUE) or not (FALSE)*/
ESOS_CHILD_TASK(esos_pic24_lcd_readByte, uint8_t *u8_data) {
  ESOS_TASK_BEGIN();
  __esos_pic24_lcd_readByte(u8_data, 0, 1);
  ESOS_TASK_END();
}

ESOS_CHILD_TASK(esos_pic24_lcd_writeByte, uint8_t u8_data) {
  ESOS_TASK_BEGIN();
  __esos_pic24_lcd_writeByte(u8_data, 1, 1);
  ESOS_TASK_END();
}

ESOS_USER_TASK(LCD_HOUSEKEEPING) {
  ESOS_TASK_BEGIN();
  static ESOS_TASK_HANDLE th_child;
  while (TRUE) {
    st_LCDmemoryBuffer.b_LCDrefreash = FALSE; // informs that the LCD might not be updated
    if(st_LCDmemoryBuffer.b_newData) {
      // set the cursor to the postion where it needs to write data
      esos_lcd_setCursor(st_LCDmemoryBuffer.u8_row, st_LCDmemoryBuffer.u8_column);
      // write the data
      ESOS_TASK_SPAWN_AND_WAIT(th_child, esos_pic24_lcd_writeByte,
                               st_LCDmemoryBuffer.u8_buffer[st_LCDmemoryBuffer.u8_bufferPosition]);
    }
    st_LCDmemoryBuffer.b_newData = FALSE; // releases the flag for new data
    st_LCDmemoryBuffer.b_LCDrefreash = TRUE; // informs that the LCD is updated
    ESOS_TASK_WAIT_TICKS(50);
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end upper_case()

/****************************************************************
** P U B L I C  F U N C T I O N S
****************************************************************/

/*  Configures (or reconfigures) the hardware-independent aspect of the LCD character module service.
Also initializes LCD service memory buffers*/
void esos_lcd_configDisplay(void) {
  // first we need to setup the MCU hardware
  esos_pic24_config_char_lcd();
  // next we will setup the MCU LCD memory buffer
  int8_t i = 0;
  while (i < 80) {
    st_LCDmemoryBuffer.u8_buffer[i] = 0;
    i++;
  }
  st_LCDmemoryBuffer.u8_row = 0;
  st_LCDmemoryBuffer.u8_column = 0;
}

/*  The LCD character module housekeeping task should perform the hardware initialization sequence
for the Hitachi 44780 devices before the service can begin its infinite loop of processing
characters and refreshing the screen. esos_lcd_init() will cause the LCD housekeeping task to
execute the LCD initialization (power-up) sequence. See pages 8-9 in the NHD0208AZ datasheet
and pages 23 and 26 in the Sitronix datasheet.*/
void esos_lcd_init(void) {
  RS_LOW();
  DELAY_MS(100);
  __esos_pic24_lcd_writeByte(LCD_WAKEUP, 0, 0); // WAKE UP!
  DELAY_MS(30);
  __esos_pic24_lcd_writeByte(LCD_WAKEUP, 0, 0);
  DELAY_MS(10);
  __esos_pic24_lcd_writeByte(LCD_WAKEUP, 0, 0);
  DELAY_MS(10);
  __esos_pic24_lcd_writeByte(LCD_WAKEUP | LCD_TWO_LINE_SETUP, 0,
                             0); // Setup 2 line interface
  __esos_pic24_lcd_writeByte(SET_DISPLAY_CONTROL | DISPLAY_ON | CURSOR_OFF |
                             BLINK_OFF, 0, 0); // Cursor on, Screen on, blink off
  st_LCDmemoryBuffer.b_cursorBlinking = FALSE;
  st_LCDmemoryBuffer.b_displayState = TRUE;
  st_LCDmemoryBuffer.b_cursorDisplay = FALSE;
  __esos_pic24_lcd_writeByte(0x06, 0, 0); // Cursor movement
  __esos_pic24_lcd_writeByte(CLEAR_DISPLAY, 0, 0); // Clear buffer
  DELAY_MS(3);
  st_LCDmemoryBuffer.b_newData = FALSE;
  //--------------------
  //esos_RegisterTask(LCD_HOUSEKEEPING);
}

// Clear LCD screen buffer and move cursor to (0,0)
void esos_lcd_clearScreen(void) {
  __esos_pic24_lcd_writeByte(CLEAR_DISPLAY, 0, 1);
  st_LCDmemoryBuffer.u8_row = 0;
  st_LCDmemoryBuffer.u8_column = 0;
}

// Move cursor to (0,0) without changing display memory buffer or the display
void esos_lcd_setCursorHome(void) {
  __esos_pic24_lcd_writeByte(SET_CURSOR_HOME, 0, 1);
  st_LCDmemoryBuffer.u8_row = 0;
  st_LCDmemoryBuffer.u8_column = 0;
}

// Move cursor to (u8_row, u8_column) without changing display memory buffer or the display
void esos_lcd_setCursor(uint8_t u8_row, uint8_t u8_column ) {
  __esos_pic24_lcd_writeByte(SET_DDRAM_ADDRESS | u8_row | (u8_column - 1), 0, 1);
  st_LCDmemoryBuffer.u8_row = u8_row;
  st_LCDmemoryBuffer.u8_column = u8_column;
}

// Writes the (single) character u8_data to the screen location (u8_row, u8_column)
void esos_lcd_writeChar(uint8_t u8_row, uint8_t u8_column, uint8_t u8_data){
  st_LCDmemoryBuffer.u8_row = u8_row;
  st_LCDmemoryBuffer.u8_column = u8_column;
  st_LCDmemoryBuffer.u8_bufferPosition = (40 * (u8_row >> 6)) + (u8_column - 1);
  st_LCDmemoryBuffer.u8_buffer[st_LCDmemoryBuffer.u8_bufferPosition]  = u8_data;
  st_LCDmemoryBuffer.b_newData = TRUE;
}

// // Returns the (single) character u8_data to the screen location (u8_row, u8_column)
uint8_t esos_lcd_getChar(uint8_t u8_row, uint8_t u8_column){
  st_LCDmemoryBuffer.u8_bufferPosition = (40 * (u8_row >> 6)) + (u8_column - 1);
  return st_LCDmemoryBuffer.u8_buffer[st_LCDmemoryBuffer.u8_bufferPosition];
}

// Writes u8_bufflen characters from pu8_data to the screen location (u8_row, u8_column)
void esos_lcd_writeBuffer(uint8_t u8_row, uint8_t u8_column, uint8_t *pu8_data, uint8_t u8_bufflen){
  while(u8_bufflen){
    esos_lcd_writeChar(u8_row, u8_column, *pu8_data);
    pu8_data++;
    u8_column++;
    u8_bufflen--;
  }
}

// Returns pu8_data with the u8_bufflen characters currently displayed starting at screen location
void esos_lcd_getBuffer(uint8_t u8_row, uint8_t u8_column, uint8_t *pu8_data, uint8_t u8_bufflen){
  static uint8_t u8_buffer_index;
  for( u8_buffer_index = 0; u8_buffer_index < u8_bufflen; u8_buffer_index++) {
    pu8_data[u8_buffer_index] = esos_lcd_getChar(u8_row, u8_column);
  }
}

//  Writes the zero-terminated string psz_data to the screen location (u8_row, u8_column)
void esos_lcd_writeString(char *psz_data, uint8_t u8_row, uint8_t u8_column){
    while (*psz_data) {
      esos_lcd_writeChar(*psz_data, u8_row, u8_column);
      u8_column++;
      psz_data++;
   }
}

// //   Set the cursor display state to the value (TRUE or FALSE) given in u8_state
void esos_lcd_setCursorDisplay(bool u8_state) {
  if(u8_state) {
    st_LCDmemoryBuffer.b_cursorDisplay = TRUE;
    __esos_pic24_lcd_writeByte(SET_DISPLAY_CONTROL | DISPLAY_ON | CURSOR_ON, 0,
                               0); // Cursor on, Screen on, blink off
  }
  else {
    st_LCDmemoryBuffer.b_cursorDisplay = FALSE;
    __esos_pic24_lcd_writeByte(SET_DISPLAY_CONTROL | DISPLAY_ON | CURSOR_OFF, 0,
                               0); // Cursor off, Screen off, blink off
  }
}

// //   Returns the cursor display state (TRUE or FALSE)
bool esos_lcd_getCursorDisplay(void) {
  return st_LCDmemoryBuffer.b_cursorDisplay;
}

// //   Set the cursor blink state to the value (TRUE or FALSE) given in u8_state
void esos_lcd_setCursorBlink(bool b_blink) {
  if(b_blink) {
    st_LCDmemoryBuffer.b_cursorBlinking = TRUE;
    __esos_pic24_lcd_writeByte(SET_DISPLAY_CONTROL | DISPLAY_ON | CURSOR_ON |
                               BLINK_ON, 0, 0); // Cursor on, Screen on, blink off
  }
  else {
    st_LCDmemoryBuffer.b_cursorBlinking = FALSE;
    __esos_pic24_lcd_writeByte(SET_DISPLAY_CONTROL | DISPLAY_ON | CURSOR_ON |
                               BLINK_OFF, 0, 0); // Cursor on, Screen on, blink off
  }
}

// // Returns the cursor display state (TRUE or FALSE)
bool esos_lcd_getCursorBlink(void) {
  return st_LCDmemoryBuffer.b_cursorBlinking;
}

// //   Set the display visible state to the value (TRUE or FALSE) given in u8_state
void esos_lcd_setDisplayVisible(bool u8_state) {
  if(u8_state) {
    st_LCDmemoryBuffer.b_displayState = TRUE;
    __esos_pic24_lcd_writeByte(SET_DISPLAY_CONTROL | DISPLAY_ON, 0,
                               0); // Cursor on, Screen on, blink off
  }
  else {
    st_LCDmemoryBuffer.b_displayState = FALSE;
    __esos_pic24_lcd_writeByte(SET_DISPLAY_CONTROL | DISPLAY_OFF, 0,
                               0); // Cursor off, Screen off, blink off
  }
}

// //   Returns the current display visible state (TRUE or FALSE)
bool esos_lcd_getDisplayVisible(void) {
  return st_LCDmemoryBuffer.b_displayState;
}

// /* Sets the custom character memory for the u8_charSlot character to the data in pu8_charData.
// Will read either eight or eleven uint8s from pu8_charData depending on the LCD character
// module settings.*/
void esos_lcd_setCustomChar(uint8_t u8_charSlot, uint8_t *pu8_charData){
  __esos_pic24_lcd_writeByte(u8_charSlot, 0, 1);
  while(*pu8_charData){
    __esos_pic24_lcd_writeByte(*pu8_charData, 1, 1);
    pu8_charData++;
  }
}

// /* Returns with pu8_charData filled with the custom character bitmap data for the custom character
// memory for the u8_charSlot character. Will fill either eight or eleven uint8s into pu8_charData
// depending on the LCD character module settings.*/
void esos_lcd_getCustomChar(uint8_t u8_charSlot, uint8_t *pu8_charData){
  static uint8_t u8_buffer_index;
  ESOS_TASK_WAIT_LCD_SET_CG_ADDRESS(u8_charSlot);
  for( u8_buffer_index = 0; u8_buffer_index < 0x08; u8_buffer_index++) {
    __esos_pic24_lcd_readByte(pu8_charData,1,1);
    pu8_charData++;
  }
}

uint8_t __get_lcd_row(void){
  return st_LCDmemoryBuffer.u8_row;
}
uint8_t __get_lcd_column(void){
  return st_LCDmemoryBuffer.u8_row;
}

bool __is_LCD_refreshed(void){
  return st_LCDmemoryBuffer.b_LCDrefreash;
}

bool __is_LCD_busy(void){
  uint8_t u8_BusyFlag;
  RS_LOW();            //RS = 0 to check busy
  configBusAsInLCD();  //set data pins all inputs
  E_HIGH();
  DELAY_US(1);
  u8_BusyFlag = BUSY_FLAG;
  E_LOW();
  DELAY_US(1);
  return !u8_BusyFlag;
}