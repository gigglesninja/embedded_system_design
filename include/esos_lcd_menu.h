// TEAM APPLE!

#ifndef ESOS_PIC24_LCD_MENU
#define ESOS_PIC24_LCD_MENU

/*** I N C L U D E S **********************************************************/
#include "../../pic24lib_all/esos/include/esos.h"
#include "esos_f15_ui.h"
#include "esos_pic24_sensor.h"
#include "esos_pic24_lcd.h"

/*** S T R U C T U R E S ******************************************************/

typedef struct menu {
  int8_t i8_menuItems;
  char *psz_titles[16];
  char *psz_lineOneInfo[16];
  char *psz_lineTwoInfo[16];
  char *psz_valueOne[16];
  char *psz_valueTwo[16];
  char *psz_valueThree[16];
  char c_id[16];
  uint8_t u8_titleType[16];
  uint8_t u8_subTitleType[16];
  uint8_t u8_SV3C[16];
  uint8_t u8_ADCChannel[16];
  uint16_t u16_lowerBound[16];
  uint16_t u16_upperBound[16];
  uint16_t u16_value[16];
  uint16_t u16_directValue[16];
  bool b_SV2C[16];
  bool b_titleHidden[16];
  bool b_extend[16];
  bool b_binary[16];
  bool b_setLM60;
  bool b_newTeam;
} menu;

// ***************** M A C R O S *********************************************************************

// arrows
#define SELECT  (0x00) // select arrow
#define UP    (0x01) // up arrow
#define DOWN  (0x02) // down arrow

// tytle types
#define SET     (0x00) // set the title value
#define READ  (0x01) // read the title value
#define STATIC  (0x02) // displays information

// subtitle set types
#define NONE  (0x00) // static read
#define SV2C  (0x01) // single value two choice select
#define SV3C  (0x02) // single value three choice select
#define DVNUM (0x03) // double value numeric select
#define SVDATA  (0x04) // single value data select

// ********************** E S O S   T A S K *************************************************************

// sets the LCD to display the title
// should only be called internally, not from external user task
#define ESOS_TASK_WAIT_DISPLAY_TITLE(title)   \
do{                       \
  ESOS_TASK_WAIT_LCD_WHILE_BUSY();      \
  esos_setTitle(title);           \
}while(0);

/*** P U B L I C - F U N C T I O N S  *****************************************/

// sets up hardware and arrows
void esos_init_menu(void);

// sets the number of menu items
void esos_create_menu(int8_t i8_numMenuItems);

// inserts a title name for title number
void esos_insert_menu_title(int8_t i8_titleNumber, uint8_t u8_titleType,
                            uint8_t u8_subTitleType, char *psz_title, char c_id);

// prints titlenumber information to LCD
void esos_setTitle(int8_t i8_titleNumber);

// sets static info for STATIC title type
void esos_setStaticInfo(int8_t i8_titleNumber, char *psz_lineOneInfo,
                        char *psz_lineTwoInfo);

// sets readout for single value two choice, value one == false, value two == true
void esos_setSV2C(int8_t i8_titleNumber, char *psz_valueOne,
                  char *psz_valueTwo);

// sets readout for single value three choice, value must be initially set by user after POR
void esos_setSV3C(int8_t i8_titleNumber, char *psz_valueOne, char *psz_valueTwo,
                  char *psz_valueThree);

void esos_setSV3C_VALUE(int8_t i8_titleNumber, uint8_t u8_value);

// sets the double value readouts, and bounds
void esos_setDVNUM(int8_t i8_titleNumber, char *psz_valueOne,
                   char *psz_valueTwo, uint16_t u16_lowerBound, uint16_t u16_upperBound);

// sets the bounds, b_extend extends to 8 bit LCD output, b_binary sets LCD output to binary dispay
void esos_setSVDATA(int8_t i8_titleNumber, uint16_t u16_lowerBound,
                    uint16_t u16_upperBound, bool b_extend, bool b_binary);

// sets the default value to data
void esos_setSVDATAValue(int8_t i8_titleNumber, uint16_t u16_value);

// returns the value of the single value two choice selection
bool esos_getSV2CValue(int8_t i8_titleNumber);

// returns the value of the single value three choice selection 1st selection is 0x01
uint8_t esos_getSV3CValue(int8_t i8_titleNumber);

// returns the value of data selections
uint16_t esos_getValue(int8_t i8_titleNumber);

// hides a title from the menu
void esos_hideMenuTitle(int8_t i8_titleNumber, bool b_hide);

// sets the sensor channel for reading data
void esos_setSensorReading(int8_t i8_titleNumber, uint8_t u8_ADCChannel, uint16_t u16_direct);

void clearNewTeam(void);
bool newTeam(void);

bool esos_getSetLM60(void);
void esos_setSetLM60(bool b_status);


#endif //endif ESOS_PIC24_LCD_MENU