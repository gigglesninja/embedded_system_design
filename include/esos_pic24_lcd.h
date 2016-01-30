// TEAM APPLE!

#ifndef ESOS_PIC24_LCD_H
#define ESOS_PIC24_LCD_H

/*** I N C L U D E S **********************************************************/
#include "../../pic24lib_all/esos/include/esos.h"
#include "revF15.h"
#include <stdbool.h>
#include <stdlib.h>

/*** S T R U C T U R E S ******************************************************/

typedef struct memoryBuffer {
  uint8_t u8_buffer[80]; 	// the memory buffer
  uint8_t u8_row;			// the row of where new data needs to be written to the LCD
  uint8_t u8_column;		// the column of where new data needs to be written to the LCD
  uint8_t u8_bufferPosition;// the location of new data in the buffer
  bool b_cursorBlinking; 	// is the cursor blinking
  bool b_cursorDisplay;		// is the cursor display on
  bool b_displayState;		// is the display on
  bool b_newData;			// is there new data in the buffer, set to true each time there is buffer data
  bool b_LCDrefreash;		// is the LCD finished refreashing, if true wait before putting data in buffer
  uint8_t *pu8_cChar;		// the pointer to the custom character address
} memoryBuffer;

// ***************** M A C R O S *********************************************************************

// To use these AND the incomming command (if applicable) then send resulting command to CONTROLLER.
// An example of this would be using set entry control for DISPLAY ON = 1, CURSOR ON = 1,
// and BLINKING CURSOR OFF = 0 input would be 0x0E AND'ED with SET_ENTRY_CONTROL. The result will be
// 0x0E which will be binary 1110 the reason for this is to ensure a user does not intend to use a command,
// and provide an input outside of the range of the specs which would result in erroneous commands. An
// example would be if the user wants to set the display to above, but provides 0x1E instead. The raw
// command would actually be sending a SET_CURSOR_OR_DISPLAY_SHIFT instead. See the datasheet for review
// http://courses.ece.msstate.edu/ece4723/datasheets/HD44780_datasheet.pdf

#define CLEAR_DISPLAY				(0x01)	// Clears Display
#define SET_CURSOR_HOME				(0x02)	// Set cursor to home position
#define SET_ENTRY_MODE				(0x04)  // Assign cursor moving direction and blinking of display
#define SET_DISPLAY_CONTROL			(0x08)  // set display, cursor, and blinking cursor
#define SET_CURSOR_OR_DISPLAY_SHIFT	(0x10)	// set cursor moving and display shift control direction
#define SET_FUNCTION				(0x20)  // set interface data lenght, number of display lines, font type
#define SET_CGRAM_ADDRESS			(0x40)  // set CGRAM address
#define SET_DDRAM_ADDRESS			(0x80)  // set DDRAM address
#define READ_BUSY_FLAG_OR_ADDRESS	(0x100) // busy flag status and address counter contents
#define WRITE_DATA 					(0x200) // write data into internal DDRAM/CGRAM
#define READ_DATA 					(0x300) // read data from DDRAM/CGRAM

#define LCD_WAKEUP					(0x30)  // Ini
#define LCD_TWO_LINE_SETUP			(0x18)	// DL = 1 = 8 bit bus, N = 1 = 2 line display, F = 0 = 5 x 8 dots
#define LCD_ONE_LINE_SETUP			(0x10)	// DL = 1 = 8 bit bus, N = 0 = 1 line display, F = 0 = 5 x 8 dots

// display control
#define BLINK_OFF					(0x00)  // CURSOR BLINK ON
#define BLINK_ON					(0x01)  // CURSOR BLINK OFF
#define CURSOR_ON					(0x02)  // CURSOR ON
#define CURSOR_OFF					(0x00)  // CURSOR ON
#define DISPLAY_ON 					(0x04)  // DISPLAY ON
#define DISPLAY_OFF 				(0x00)  // DISPLAY OFF

// Screen information
#define ROW_ONE 					(0x00) 	// Starts Cursor on row 1
#define ROW_TWO						(0x40)	// Starts Cursor on row 2

// ********************** E S O S   T A S K *************************************************************
// RS=0; D=0x00-0xEF
#define ESOS_TASK_WAIT_LCD_WRITE_COMMAND(u8_cmd)						\
do{																		\
	if(u8_cmd == READ_BUSY_FLAG_OR_ADDRESS){							\
		__esos_pic24_lcd_writeByte(u8_cmd, 0, 0);						\
	}																	\
	else{																\
		__esos_pic24_lcd_writeByte(u8_cmd, 0, 1);						\
	}																	\
}while(0)

// RS=0; D=0x40-0x7F
#define ESOS_TASK_WAIT_LCD_SET_CG_ADDRESS(u8_addr)						\
do{																		\
	__esos_pic24_lcd_writeByte(u8_addr, 1 , 1);							\
}while(0)

// RS=0; D=0x80-0xFF
#define ESOS_TASK_WAIT_LCD_SET_DATA_ADDRESS(u8_addr)					\
do{																		\
	__esos_pic24_lcd_writeByte(u8_addr, 1, 1);							\
}while(0)

// RS=0;
#define ESOS_TASK_WAIT_LCD_WHILE_BUSY()	ESOS_TASK_WAIT_UNTIL(__is_LCD_busy())

// RS=0;
#define ESOS_TASK_WAIT_LCD_READ_ADDRESS(u8_addr)						\
do{																		\
	__esos_pic24_lcd_readByte((&u8_addr), 0, 1);						\
}while(0)

// RS=1;
#define ESOS_TASK_WAIT_LCD_WRITE_DATA(u8_data)							\
do{																		\
	__esos_pic24_lcd_writeByte(u8_data, 1, 1);							\
}while(0)

// RS=1;
#define ESOS_TASK_WAIT_LCD_READ_DATA(u8_addr)							\
do{																		\
	__esos_pic24_lcd_readByte((&u8_addr), 1, 1);						\
}while(0)

/*Causes the current task to block until the LCD character module service housekeeping task has
updated the screen completely.*/
#define ESOS_TASK_WAIT_ON_LCD_REFRESH()	ESOS_TASK_WAIT_UNTIL(__is_LCD_refreshed())

/*** P U B L I C - F U N C T I O N S  *****************************************/

/*	Configures (or reconfigures) the hardware-independent aspect of the LCD character module service.
Also initializes LCD service memory buffers*/
void esos_lcd_configDisplay(void);

/*	The LCD character module housekeeping task should perform the hardware initialization sequence
for the Hitachi 44780 devices before the service can begin its infinite loop of processing
characters and refreshing the screen. esos_lcd_init() will cause the LCD housekeeping task to
execute the LCD initialization (power-up) sequence. See pages 8-9 in the NHD0208AZ datasheet
and pages 23 and 26 in the Sitronix datasheet.*/
void esos_lcd_init(void);

// Clear LCD screen buffer and move cursor to (0,0)
void esos_lcd_clearScreen(void);

// Move cursor to (0,0) without changing display memory buffer or the display
void esos_lcd_setCursorHome(void);

// Move cursor to (u8_row, u8_column) without changing display memory buffer or the display
void esos_lcd_setCursor(uint8_t u8_row, uint8_t u8_column );

// Writes the (single) character u8_data to the screen location (u8_row, u8_column)
void esos_lcd_writeChar(uint8_t u8_row, uint8_t u8_column, uint8_t u8_data);

// Returns the character currently displayed at screen location (u8_row, u8_column)
uint8_t esos_lcd_getChar(uint8_t u8_row, uint8_t u8_column);

// Writes u8_bufflen characters from pu8_data to the screen location (u8_row, u8_column)
void esos_lcd_writeBuffer(uint8_t u8_row, uint8_t u8_column, uint8_t *pu8_data, uint8_t u8_bufflen);

// Returns pu8_data with the u8_bufflen characters currently displayed starting at screen location
void esos_lcd_getBuffer(uint8_t u8_row, uint8_t u8_column, uint8_t *pu8_data, uint8_t u8_bufflen);

// 	Writes the zero-terminated string psz_data to the screen location (u8_row, u8_column)
void esos_lcd_writeString(char *psz_data, uint8_t u8_row, uint8_t u8_column);

// 	Set the cursor display state to the value (TRUE or FALSE) given in u8_state
void esos_lcd_setCursorDisplay(bool b_state);

// 	Returns the cursor display state (TRUE or FALSE)
bool esos_lcd_getCursorDisplay(void);

// 	Set the cursor blink state to the value (TRUE or FALSE) given in u8_state
void esos_lcd_setCursorBlink(bool b_blink);

//	Returns the cursor display state (TRUE or FALSE)
bool esos_lcd_getCursorBlink(void);

// 	Set the display visible state to the value (TRUE or FALSE) given in u8_state
void esos_lcd_setDisplayVisible(bool u8_state);

// 	Returns the current display visible state (TRUE or FALSE)
bool esos_lcd_getDisplayVisible(void);

/*	Sets the custom character memory for the u8_charSlot character to the data in pu8_charData.
Will read either eight or eleven uint8s from pu8_charData depending on the LCD character
module settings.*/
void esos_lcd_setCustomChar(uint8_t u8_charSlot, uint8_t *pu8_charData);

/*	Returns with pu8_charData filled with the custom character bitmap data for the custom character
memory for the u8_charSlot character. Will fill either eight or eleven uint8s into pu8_charData
depending on the LCD character module settings.*/
void esos_lcd_getCustomChar(uint8_t u8_charSlot, uint8_t *pu8_charData);

uint8_t __get_lcd_row(void);
uint8_t __get_lcd_column(void);
bool __is_LCD_refreshed(void);
bool __is_LCD_busy(void);

#endif //endif ESOS_PIC24_LCD_H