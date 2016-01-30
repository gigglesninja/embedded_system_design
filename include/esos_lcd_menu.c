// TEAM APPLE!

#include    "esos_lcd_menu.h"

// ******** G L O B A L S ***************
#define LM60_OFFSET             0xA5A0   // 424 * 100
#define LM60_CELSIUS_CONSTANT   0x0271   // 6.25 *100

volatile menu st_menu; // the structure which holds the values

/****************************************************************
** P U B L I C  F U N C T I O N S
****************************************************************/
// sets the number of items in the menu
void esos_create_menu(int8_t i8_numMenuItems) {
  st_menu.i8_menuItems = i8_numMenuItems;
}

bool esos_getSetLM60(void){
  return st_menu.b_setLM60;
}

void esos_setSetLM60(bool b_status){
  st_menu.b_setLM60 = b_status;
}

void clearNewTeam(void){
  st_menu.b_newTeam = FALSE;
}

bool newTeam(void){
  return st_menu.b_newTeam;
}

// set the menu title, the title type, where the title will be displayed in the menu and the subtitle type
void esos_insert_menu_title(int8_t i8_titleNumber, uint8_t u8_titleType,
                            uint8_t u8_subTitleType, char *psz_title, char c_id) {
  if(i8_titleNumber > 0 && i8_titleNumber <= st_menu.i8_menuItems) {
    st_menu.psz_titles[i8_titleNumber - 1] = psz_title;
    st_menu.u8_titleType[i8_titleNumber - 1] = u8_titleType;
    st_menu.u8_subTitleType[i8_titleNumber - 1] = u8_subTitleType;
    st_menu.c_id[i8_titleNumber - 1] = c_id;
  }
}

// set the LCD to display the current title selected
void esos_setTitle(int8_t i8_titleNumber) {
  esos_lcd_clearScreen(); // clears any data off screen
  static char *psz_title; // creates a temp value to print to screen
  esos_lcd_setCursor(ROW_ONE, 1 ); // sets cursor to home
  if(st_menu.u8_titleType[i8_titleNumber - 1] == SET) {
    if(i8_titleNumber != 1){
        ESOS_TASK_WAIT_LCD_WRITE_DATA(st_menu.c_id[i8_titleNumber - 1]);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
    }
    ESOS_TASK_WAIT_LCD_WRITE_DATA('S');
    ESOS_TASK_WAIT_LCD_WRITE_DATA('e');
    ESOS_TASK_WAIT_LCD_WRITE_DATA('t');
  }
  else if(st_menu.u8_titleType[i8_titleNumber - 1] == READ) {
    ESOS_TASK_WAIT_LCD_WRITE_DATA(st_menu.c_id[i8_titleNumber - 1]);
    ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
    ESOS_TASK_WAIT_LCD_WRITE_DATA('R');
    ESOS_TASK_WAIT_LCD_WRITE_DATA('e');
    ESOS_TASK_WAIT_LCD_WRITE_DATA('a');
    ESOS_TASK_WAIT_LCD_WRITE_DATA('d');
  }
  esos_lcd_setCursor(ROW_TWO, 1 ); // sets cursor to home
  psz_title = st_menu.psz_titles[i8_titleNumber - 1];
  while(*psz_title) { // print the value to the screen
    ESOS_TASK_WAIT_LCD_WRITE_DATA(*psz_title);
    psz_title++;
  }
}

// sets the line info for a static menu sub item
void esos_setStaticInfo(int8_t i8_titleNumber, char *psz_lineOneInfo,
                        char *psz_lineTwoInfo) {
  if(i8_titleNumber > 0 && i8_titleNumber <= st_menu.i8_menuItems) {
    st_menu.psz_lineOneInfo[i8_titleNumber - 1] = psz_lineOneInfo;
    st_menu.psz_lineTwoInfo[i8_titleNumber - 1] = psz_lineTwoInfo;
  }
}

// sets readout for single value two choice
void esos_setSV2C(int8_t i8_titleNumber, char *psz_valueOne,
                  char *psz_valueTwo) {
  if(i8_titleNumber > 0 && i8_titleNumber <= st_menu.i8_menuItems) {
    st_menu.psz_valueOne[i8_titleNumber - 1] = psz_valueOne;
    st_menu.psz_valueTwo[i8_titleNumber - 1] = psz_valueTwo;
  }
}

// sets readout for single value three choice
void esos_setSV3C(int8_t i8_titleNumber, char *psz_valueOne, char *psz_valueTwo,
                  char *psz_valueThree) {
  if(i8_titleNumber > 0 && i8_titleNumber <= st_menu.i8_menuItems) {
    st_menu.psz_valueOne[i8_titleNumber - 1] = psz_valueOne;
    st_menu.psz_valueTwo[i8_titleNumber - 1] = psz_valueTwo;
    st_menu.psz_valueThree[i8_titleNumber - 1] = psz_valueThree;
  }
}

// returns the value of the single value two choice selection
bool esos_getSV2CValue(int8_t i8_titleNumber) {
  return st_menu.b_SV2C[i8_titleNumber - 1];
}

// returns the value of the single value three choice selection 1st selection is 0x00
uint8_t esos_getSV3CValue(int8_t i8_titleNumber) {
  return st_menu.u8_SV3C[i8_titleNumber - 1];
}

void esos_setSV3C_VALUE(int8_t i8_titleNumber, uint8_t u8_value){
  st_menu.u8_SV3C[i8_titleNumber - 1] = u8_value;
}

// need to finish this
// void esos_setDVNUM(int8_t i8_titleNumber, char *psz_valueOne,
//                    char *psz_valueTwo, uint16_t u16_lowerBound, uint16_t u16_upperBound){
//   if(i8_titleNumber > 0 && i8_titleNumber <= st_menu.i8_menuItems) {
//     st_menu.psz_valueOne[i8_titleNumber - 1] = psz_valueOne;
//     st_menu.psz_valueTwo[i8_titleNumber - 1] = psz_valueTwo;
//     st_menu.u16_lowerBound[i8_titleNumber - 1] = u16_lowerBound;
//     st_menu.u16_upperBound[i8_titleNumber - 1] = u16_upperBound;
//     st_menu.u16_value[i8_titleNumber - 1] = u16_lowerBound;
//   }
// }

// sets lower and upper bounds to a single value data title
void esos_setSVDATA(int8_t i8_titleNumber, uint16_t u16_lowerBound,
                    uint16_t u16_upperBound, bool b_extend, bool b_binary) {
  if(i8_titleNumber > 0 && i8_titleNumber <= st_menu.i8_menuItems) {
    st_menu.u16_lowerBound[i8_titleNumber - 1] = u16_lowerBound;
    st_menu.u16_upperBound[i8_titleNumber - 1] = u16_upperBound;
    st_menu.u16_value[i8_titleNumber - 1] = u16_lowerBound;
    st_menu.b_extend[i8_titleNumber - 1] = b_extend;
    st_menu.b_binary[i8_titleNumber - 1] = b_binary;
  }
}

// set a default value to a single value data
void esos_setSVDATAValue(int8_t i8_titleNumber, uint16_t u16_value) {
  if(i8_titleNumber > 0 && i8_titleNumber <= st_menu.i8_menuItems) {
    st_menu.u16_value[i8_titleNumber - 1] = u16_value;
  }
}

// returns the data value of a title
uint16_t esos_getValue(int8_t i8_titleNumber) {
  return st_menu.u16_value[i8_titleNumber - 1];
}

// hides a title in the menu
void esos_hideMenuTitle(int8_t i8_titleNumber, bool b_hide) {
  st_menu.b_titleHidden[i8_titleNumber - 1] = b_hide;
}

// sets the ADC sensor channel
void esos_setSensorReading(int8_t i8_titleNumber, uint8_t u8_ADCChannel, uint16_t u16_direct) {
  if(i8_titleNumber > 0 && i8_titleNumber <= st_menu.i8_menuItems) {
    st_menu.u8_ADCChannel[i8_titleNumber - 1] = u8_ADCChannel;
    st_menu.u16_directValue[i8_titleNumber - 1] = u16_direct;

  }
}

/****************************************************************
** P R I V A T E  F U N C T I O N S
****************************************************************/
// set LCD static out info
void __lineStaticInfo(int8_t i8_titleNumber) {
  static char *psz_info;
  psz_info = st_menu.psz_lineOneInfo[i8_titleNumber - 1];
  esos_lcd_setCursor(ROW_ONE, 1 );
  while(*psz_info) { // print the value to the screen
    ESOS_TASK_WAIT_LCD_WRITE_DATA(*psz_info);
    psz_info++;
  }
  psz_info = st_menu.psz_lineTwoInfo[i8_titleNumber - 1];
  esos_lcd_setCursor(ROW_TWO, 1 );
  while(*psz_info) { // print the value to the screen
    ESOS_TASK_WAIT_LCD_WRITE_DATA(*psz_info);
    psz_info++;
  }
}

// set LCD SV2 out info
void __lineSV2CInfo(int8_t i8_titleNumber) {
  static char *psz_info;
  psz_info = st_menu.psz_valueOne[i8_titleNumber - 1];
  esos_lcd_setCursor(ROW_ONE, 1 );
  while(*psz_info) { // print the value to the screen
    ESOS_TASK_WAIT_LCD_WRITE_DATA(*psz_info);
    psz_info++;
  }
  psz_info = st_menu.psz_valueTwo[i8_titleNumber - 1];
  esos_lcd_setCursor(ROW_TWO, 1 );
  while(*psz_info) { // print the value to the screen
    ESOS_TASK_WAIT_LCD_WRITE_DATA(*psz_info);
    psz_info++;
  }
}

// set SV3C LCD out info
void __lineSV3CInfo(int8_t i8_titleNumber, uint8_t u8_value) {
  static char *psz_info;;
  psz_info = st_menu.psz_titles[i8_titleNumber - 1];
  esos_lcd_setCursor(ROW_ONE, 1 );
  while(*psz_info) { // print the value to the screen
    ESOS_TASK_WAIT_LCD_WRITE_DATA(*psz_info);
    psz_info++;
  }
  if(u8_value == 1) {
    psz_info = st_menu.psz_valueOne[i8_titleNumber - 1];
  }
  else if(u8_value == 2) {
    psz_info = st_menu.psz_valueTwo[i8_titleNumber - 1];
  }
  else {
    psz_info = st_menu.psz_valueThree[i8_titleNumber - 1];
  }
  esos_lcd_setCursor(ROW_TWO, 1 );
  while(*psz_info) { // print the value to the screen
    ESOS_TASK_WAIT_LCD_WRITE_DATA(*psz_info);
    psz_info++;
  }
}

// set DVNUM LCD out info
void __lineDVNUMInfo(int8_t i8_titleNumber) {
  static char *psz_info;
  psz_info = st_menu.psz_valueOne[i8_titleNumber - 1];
  esos_lcd_setCursor(ROW_ONE, 1 );
  while(*psz_info) { // print the value to the screen
    ESOS_TASK_WAIT_LCD_WRITE_DATA(*psz_info);
    psz_info++;
  }
  psz_info = st_menu.psz_valueTwo[i8_titleNumber - 1];
  esos_lcd_setCursor(ROW_TWO, 1 );
  while(*psz_info) { // print the value to the screen
    ESOS_TASK_WAIT_LCD_WRITE_DATA(*psz_info);
    psz_info++;
  }
}

// set SVDATA LCD out info
void __lineSVDATAInfo(int8_t i8_titleNumber) {
  static char *psz_info;;
  psz_info = st_menu.psz_titles[i8_titleNumber - 1];
  esos_lcd_setCursor(ROW_ONE, 1 );
  while(*psz_info) { // print the value to the screen
    ESOS_TASK_WAIT_LCD_WRITE_DATA(*psz_info);
    psz_info++;
  }
}

// converts the hex to a char
char __u8_hexToChar(uint8_t u8_char) {
  if(u8_char == 0x0) {
    return '0';
  }
  else if(u8_char == 0x1) {
    return '1';
  }
  else if(u8_char == 0x2) {
    return '2';
  }
  else if(u8_char == 0x3) {
    return '3';
  }
  else if(u8_char == 0x4) {
    return '4';
  }
  else if(u8_char == 0x5) {
    return '5';
  }
  else if(u8_char == 0x6) {
    return '6';
  }
  else if(u8_char == 0x7) {
    return '7';
  }
  else if(u8_char == 0x8) {
    return '8';
  }
  return '9';
}

// overflow happens at 10000!!!!
// only handles from 0 - 9999
uint16_t __hexToValue(uint16_t u16_value) {
  uint16_t u16_count;
  uint16_t u16_check;
  u16_count = 0x00;
  while(u16_value > 0x0A) {
    u16_count = u16_count + 0x10;
    u16_value = u16_value - 0x0A;
    if(u16_count == 0xA0) {
      u16_count = 0x100;
    }
    // *******************************
    // need to roll over from F0 to 60
    u16_check = u16_count & 0x00F0;
    if(u16_check == 0x00F0) {
      u16_count = u16_count + 0x060;
    }
    // *******************************
    // need to roll over from F00 to 600
    u16_check = u16_count & 0x0F00;
    if(u16_check == 0x0F00) {
      u16_count = u16_count + 0x0600;
    }
  }
  u16_count = u16_count + u16_value;
  // ************************************************
  // CONVERT 0xA
  u16_check = u16_count & 0x0F;
  if(u16_check == 0x0A) {
    u16_count = u16_count + 0x0010;
    u16_count = u16_count & 0xFFF0;
  }
  // ************************************************
  // ************************************************
  // CONVERT 0xA0 - 0xF0
  // Handles from 200 - 999
  u16_check = u16_count & 0x00F0;
  if(u16_check == 0x00A0) {
    u16_count = u16_count + 0x0100;
    u16_count = u16_count & 0xFF0F;
  }
  else if(u16_check == 0x00B0) {
    u16_count = u16_count + 0x0100;
    u16_count = u16_count & 0xFF0F;
    u16_count = u16_count + 0x0010;
  }
  else if(u16_check == 0x00C0) {
    u16_count = u16_count + 0x0100;
    u16_count = u16_count & 0xFF0F;
    u16_count = u16_count + 0x0020;
  }
  else if(u16_check == 0x00D0) {
    u16_count = u16_count + 0x0100;
    u16_count = u16_count & 0xFF0F;
    u16_count = u16_count + 0x0030;
  }
  else if(u16_check == 0x00E0) {
    u16_count = u16_count + 0x0100;
    u16_count = u16_count & 0xFF0F;
    u16_count = u16_count + 0x0040;
  }
  else if(u16_check == 0x00F0) {
    u16_count = u16_count + 0x0100;
    u16_count = u16_count & 0xFF0F;
    u16_count = u16_count + 0x0050;
  }
  // ************************************************
  // CONVERT 0xA00 - 0xF00
  // Handles from 1000 - 9999
  u16_check = u16_count & 0x0F00;
  if(u16_check == 0x0A00) {
    u16_count = u16_count + 0x1000;
    u16_count = u16_count & 0xF0FF;
  }
  else if(u16_check == 0x0B00) {
    u16_count = u16_count + 0x1000;
    u16_count = u16_count & 0xF0FF;
    u16_count = u16_count + 0x0100;
  }
  else if(u16_check == 0x0C00) {
    u16_count = u16_count + 0x1000;
    u16_count = u16_count & 0xF0FF;
    u16_count = u16_count + 0x0200;
  }
  else if(u16_check == 0x0D00) {
    u16_count = u16_count + 0x1000;
    u16_count = u16_count & 0xF0FF;
    u16_count = u16_count + 0x0300;
  }
  else if(u16_check == 0x0E00) {
    u16_count = u16_count + 0x1000;
    u16_count = u16_count & 0xF0FF;
    u16_count = u16_count + 0x0400;
  }
  else if(u16_check == 0x0F00) {
    u16_count = u16_count + 0x1000;
    u16_count = u16_count & 0xF0FF;
    u16_count = u16_count + 0x0500;
  }
  return u16_count;
}

// returns the hex value representation of the temperature (for LCD purpose)
// might be able to use the other function. Have not tried it yet
uint8_t __u8_hexToTemp(uint8_t u8_upper, uint8_t u8_lower, bool b_fahrenheit) {
  uint8_t u8_data;
  u8_upper = u8_upper * 16;
  u8_data = u8_upper + u8_lower;
  if(u8_data <= 20) {
    if(b_fahrenheit) {
      return 0x68;
    }
    return 0x20;
  }
  else if(u8_data == 0x15) {
    if(b_fahrenheit) {
      return 0x69;
    }
    return 0x21;
  }
  else if(u8_data == 0x16) {
    if(b_fahrenheit) {
      return 0x72;
    }
    return 0x22;
  }
  else if(u8_data == 0x17) {
    if(b_fahrenheit) {
      return 0x73;
    }
    return 0x23;
  }
  else if(u8_data == 0x18) {
    if(b_fahrenheit) {
      return 0x75;
    }
    return 0x24;
  }
  else if(u8_data == 0x19) {
    if(b_fahrenheit) {
      return 0x77;
    }
    return 0x25;
  }
  else if(u8_data == 0x1a) {
    if(b_fahrenheit) {
      return 0x79;
    }
    return 0x26;
  }
  else if(u8_data == 0x1b) {
    if(b_fahrenheit) {
      return 0x81;
    }
    return 0x27;
  }
  else if(u8_data == 0x1c) {
    if(b_fahrenheit) {
      return 0x82;
    }
    return 0x28;
  }
  else if(u8_data == 0x1d) {
    if(b_fahrenheit) {
      return 0x84;
    }
    return 0x29;
  }
  else if(u8_data == 0x1e) {
    if(b_fahrenheit) {
      return 0x86;
    }
    return 0x30;
  }
  else if(u8_data == 0x1f) {
    if(b_fahrenheit) {
      return 0x88;
    }
    return 0x31;
  }
  else if(u8_data == 0x20) {
    if(b_fahrenheit) {
      return 0x90;
    }
    return 0x32;
  }
  else if(u8_data == 0x21) {
    if(b_fahrenheit) {
      return 0x91;
    }
    return 0x33;
  }
  else if(u8_data == 0x22) {
    if(b_fahrenheit) {
      return 0x93;
    }
    return 0x34;
  }
  if(b_fahrenheit) {
    return 0x95;
  }
  return 0x35;
}

// sets the read title LCD child task constants
void __lineREADInfo(int8_t i8_titleNumber) {
  static char *psz_info;;
  psz_info = st_menu.psz_titles[i8_titleNumber - 1];
  esos_lcd_setCursor(ROW_ONE, 1 );
  while(*psz_info) { // print the value to the screen
    ESOS_TASK_WAIT_LCD_WRITE_DATA(*psz_info);
    psz_info++;
  }
}

/****************************************************************
** E S O S  T A S K
****************************************************************/
// STATIC DISPLAY
ESOS_CHILD_TASK(staticDisplay, int8_t i8_titleNumber) {
  ESOS_TASK_BEGIN();
  static bool b_stay = TRUE; // determains the need to keep child task runing
  esos_lcd_clearScreen(); // clear the screen
  __lineStaticInfo(i8_titleNumber); // set the static output
  while(b_stay) {
    if(f15ui_isRpgTurningCW()) { // Scroll Right
      ESOS_TASK_WAIT_LCD_WRITE_COMMAND(0x18);
    }
    else if(f15ui_isRpgTurningCCW()) { // Scroll Left
      ESOS_TASK_WAIT_LCD_WRITE_COMMAND(0x1C);
    }
    if(f15ui_isSW3Pressed()) { // see if need to break out of child task
      b_stay = false;
    }
    ESOS_TASK_WAIT_TICKS(20);
  }
  b_stay = TRUE; // make sure we set this back to true so we can return if needed
  ESOS_TASK_END();
}

// SINGLE VALUE DOUBLE SELECT
ESOS_CHILD_TASK(sv2c, int8_t i8_titleNumber) {
  ESOS_TASK_BEGIN();
  static bool b_stay = TRUE; // determains the need to keep child running
  static bool b_selection; // keeps up with the selection to return to the user
  esos_lcd_clearScreen(); // clear the screen
  esos_lcd_setCursor(ROW_ONE, 8 ); // print an initial selection arrow
  ESOS_TASK_WAIT_LCD_WRITE_DATA(SELECT);
  __lineSV2CInfo(i8_titleNumber); // print the selections choices
  while(b_stay) {
    // logic for scrolling and making the selection
    // choice two is a true
    if(f15ui_isRpgTurningCW() || f15ui_isSW1Pressed()) {
      esos_lcd_setCursor(ROW_ONE, 8 );
      ESOS_TASK_WAIT_LCD_WRITE_DATA(SELECT);
      esos_lcd_setCursor(ROW_TWO, 8 );
      ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
      b_selection = FALSE;
    }
    else if(f15ui_isRpgTurningCCW() || f15ui_isSW2Pressed()) {
      esos_lcd_setCursor(ROW_ONE, 8 );
      ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
      esos_lcd_setCursor(ROW_TWO, 8 );
      ESOS_TASK_WAIT_LCD_WRITE_DATA(SELECT);
      b_selection = TRUE;
    }
    if(f15ui_isSW3Pressed()) { // see if we need to break out of child task
      b_stay = false;
      st_menu.b_SV2C[i8_titleNumber - 1] = b_selection; // set the selection
    }
    ESOS_TASK_WAIT_TICKS(20);
  }
  b_stay = TRUE; // make sure we set this back to true so we can return if needed
  ESOS_TASK_END();
}

// SINGLE VALUE TRIPLE SELECT
ESOS_CHILD_TASK(sv3c, int8_t i8_titleNumber) {
  ESOS_TASK_BEGIN();
  static bool b_stay = TRUE; // check to see if the child loops
  static bool b_update; // for updating arrows
  static uint8_t u8_value; // set initial value
  u8_value = st_menu.u8_SV3C[i8_titleNumber - 1];
  esos_lcd_clearScreen(); // clear the screen of artifacts
  __lineSV3CInfo(i8_titleNumber, u8_value);
  esos_lcd_setCursor(ROW_TWO, 7);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
  ESOS_TASK_WAIT_LCD_WRITE_DATA(DOWN);
  while(b_stay) {
    // handles the logic for scrolling though sub menu and
    // setting value of user selection
    if(f15ui_isSW1Pressed() || f15ui_isRpgTurningCCW()) {
      if(u8_value > 1) {
        u8_value--;
        __lineSV3CInfo(i8_titleNumber, u8_value);
        u8_value = u8_value;
        b_update = TRUE;
      }
    }
    else if(f15ui_isSW2Pressed() || f15ui_isRpgTurningCW()) {
      if(u8_value < 3) {
        u8_value++;
        __lineSV3CInfo(i8_titleNumber, u8_value);
        u8_value = u8_value;
        b_update = TRUE;
      }
    }
    // if an update has happen, make sure to update the arrows appropriately
    if(b_update) {
      if(u8_value == 1) {
        esos_lcd_setCursor(ROW_ONE, 8);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
        esos_lcd_setCursor(ROW_TWO, 7);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
        ESOS_TASK_WAIT_LCD_WRITE_DATA(DOWN);
      }
      else if(u8_value == 2) {
        esos_lcd_setCursor(ROW_ONE, 8);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(UP);
        esos_lcd_setCursor(ROW_TWO, 7);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
        ESOS_TASK_WAIT_LCD_WRITE_DATA(DOWN);
      }
      else {
        esos_lcd_setCursor(ROW_ONE, 8);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(UP);
        esos_lcd_setCursor(ROW_TWO, 8);
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
      }
    }
    if(f15ui_isSW3Pressed()) { // see if we need to break out of child task
      b_stay = false; // break out of child task
      st_menu.u8_SV3C[i8_titleNumber - 1] = u8_value;  // set the value
      if(i8_titleNumber == 1){
        st_menu.b_newTeam = TRUE;
        ESOS_TASK_WAIT_UNTIL(st_menu.b_newTeam == FALSE);
      }
    }
    ESOS_TASK_WAIT_TICKS(20);
  }
  b_stay = TRUE; // make sure we set this back to true so we can return if needed
  ESOS_TASK_END();
}

// single value data task
ESOS_CHILD_TASK(svdata, int8_t i8_titleNumber) {
  ESOS_TASK_BEGIN();
  static bool b_stay = TRUE; // determains the need to keep child task runing
  static uint16_t u16_value;
  static uint16_t u16_lcdValue;
  static uint8_t u8_upper1; // 0xF000
  static uint8_t u8_upper2; // 0x0F00
  static uint8_t u8_lower1; // 0x00F0
  static uint8_t u8_lower2; // 0x000F
  u16_value = st_menu.u16_value[i8_titleNumber - 1];
  esos_lcd_clearScreen(); // clear the screen
  __lineSVDATAInfo(i8_titleNumber); // set the static output
  while(b_stay) {
    // make sure the limits are not broken
    if(f15ui_isSW1Pressed()) { 
      if(u16_value < st_menu.u16_upperBound[i8_titleNumber - 1]) {
        u16_value++;
      }
    }
    else if(f15ui_isSW2Pressed()) { 
      if(u16_value > st_menu.u16_lowerBound[i8_titleNumber - 1]) {
        u16_value--;
      }
    }
    // for fast tuning the value
    if(f15ui_isRpgTurningCW()) {
      if(u16_value < st_menu.u16_upperBound[i8_titleNumber - 1]) {
        u16_value = u16_value + 50;
        if(u16_value > st_menu.u16_upperBound[i8_titleNumber - 1]) {
          u16_value = st_menu.u16_upperBound[i8_titleNumber - 1];
        }
      }
    }
    else if(f15ui_isRpgTurningCCW()) { // Scroll Left
      if(u16_value > st_menu.u16_lowerBound[i8_titleNumber - 1]) {
        u16_value = u16_value - 50;
        if(u16_value < st_menu.u16_lowerBound[i8_titleNumber - 1]) {
          u16_value = st_menu.u16_lowerBound[i8_titleNumber - 1];
        }
      }
    }
    u16_lcdValue = __hexToValue(u16_value);
    u8_upper1 = u16_lcdValue >> 12;
    u8_upper1 = u8_upper1 & 0x0F;
    u8_upper2 = u16_lcdValue >> 8;
    u8_upper2 = u8_upper2 & 0x0F;
    u8_lower1 = (uint8_t)u16_lcdValue >> 4;
    u8_lower1 = u8_lower1 & 0x0F;
    u8_lower2 = (uint8_t)u16_lcdValue & 0x0F;

    // ************* B I N A R Y  O U T P U T ***********************************
    // This will only work for upper 3 bits
    // The instructions for how the LED's should work does not make sense
    // when compared to the other sections of single data entry values.
    // It seems it would be easier to have a dedicated binary menu input option
    // then trying to do it this way.
    if(st_menu.b_binary[i8_titleNumber - 1]) {
      if(u8_lower2 == 0x02) {
        u8_lower1 = 0x0;
        u8_upper2 = 0x1;
        u8_upper1 = 0x0;
        u16_value = 0x02;
      }
      else if(u8_lower2 == 0x03) {
        u8_lower1 = 0x1;
        u8_upper2 = 0x1;
        u8_upper1 = 0x0;
        u16_value = 0x03;
      }
      else if(u8_lower2 == 0x04) {
        u8_lower1 = 0x0;
        u8_upper2 = 0x0;
        u8_upper1 = 0x1;
        u16_value = 0x04;
      }
      else if(u8_lower2 == 0x05) {
        u8_lower1 = 0x1;
        u8_upper2 = 0x0;
        u8_upper1 = 0x1;
        u16_value = 0x05;
      }
      else if(u8_lower2 == 0x06) {
        u8_lower1 = 0x0;
        u8_upper2 = 0x1;
        u8_upper1 = 0x1;
        u16_value = 0x06;
      }
      else if(u8_lower2 == 0x07) {
        u8_lower1 = 0x1;
        u8_upper2 = 0x1;
        u8_upper1 = 0x1;
        u16_value = 0x07;
      }
      else if(u8_lower2 == 0x01 || u16_value > 0x00) {
        u8_lower1 = 0x1;
        u8_upper2 = 0x0;
        u8_upper1 = 0x0;
        u16_value = 0x01;
      }
      u8_lower2 = 0;
    }
    esos_lcd_setCursor(ROW_TWO, 1 ); // move cursor to upper byte position
    ESOS_TASK_WAIT_LCD_WRITE_DATA(__u8_hexToChar(u8_upper1)); // write upper byte
    ESOS_TASK_WAIT_LCD_WRITE_DATA(__u8_hexToChar(u8_upper2)); // write middle byte
    ESOS_TASK_WAIT_LCD_WRITE_DATA(__u8_hexToChar(u8_lower1)); // write lower byte
    ESOS_TASK_WAIT_LCD_WRITE_DATA(__u8_hexToChar(u8_lower2)); // write lower byte
    // *************************for later use ************************
    if(st_menu.b_extend[i8_titleNumber - 1]) {
      ESOS_TASK_WAIT_LCD_WRITE_DATA('0');
      ESOS_TASK_WAIT_LCD_WRITE_DATA('0');
      ESOS_TASK_WAIT_LCD_WRITE_DATA('0');
      ESOS_TASK_WAIT_LCD_WRITE_DATA('0');
    }
    // ************* B I N A R Y  O U T P U T ***********************************

    if(f15ui_isSW3Pressed()) { // see if need to break out of child task
      b_stay = false;
    }
    ESOS_TASK_WAIT_TICKS(35);
  }
  st_menu.u16_value[i8_titleNumber - 1] = u16_value;
  b_stay = TRUE; // make sure we set this back to true so we can return if needed
  ESOS_TASK_END();
}

// THIS IS ONLY CODED FOR ADC SENSOR 3 RIGHT NOW
// NEED TO ADD CODE FOR OTHER CHANNELS IF NEEDED
ESOS_CHILD_TASK(read, int8_t i8_titleNumber) {
  ESOS_TASK_BEGIN();
  static bool b_stay = TRUE; // determains the need to keep child task runing
  static bool b_changeFormat = FALSE;
  static uint16_t u16_data;
  static uint8_t u8_temp;
  static uint8_t u8_slider;
  static uint8_t u8_oldtemp;
  static uint8_t u8_upper;
  static uint8_t u8_lower;
  esos_lcd_clearScreen(); // clear the screen
  __lineREADInfo(i8_titleNumber); // set the static output
  while(b_stay) {
    // FOR LM60 READING
    if(st_menu.u8_ADCChannel[i8_titleNumber - 1] == ESOS_SENSOR_CH03) {
      ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(st_menu.u8_ADCChannel[i8_titleNumber - 1],
                                       ESOS_SENSOR_VREF_3V3);
      ESOS_USE_ADC_12_BIT(TRUE);
      ESOS_TASK_WAIT_SENSOR_READ(u16_data, ESOS_SENSOR_AVG64,
                                 ESOS_SENSOR_FORMAT_VOLTAGE); // get the ADC reading
      ESOS_TASK_WAIT_UNTIL(ESOS_SENSOR_CLOSE());
      u16_data = (u16_data * 10) - LM60_OFFSET;
      u16_data = u16_data / LM60_CELSIUS_CONSTANT;
      // first we need to separate the powers
      u8_upper = (uint8_t)u16_data & 0xF0;
      u8_upper = u8_upper >> 4;
      u8_lower = (uint8_t)u16_data & 0x0F;
      // next convert it to a hex decimal representation
      u8_temp = __u8_hexToTemp(u8_upper,u8_lower, b_changeFormat);
      // so the slider code does not have to be changed use this
      u8_slider = __u8_hexToTemp(u8_upper,u8_lower, FALSE);
      u8_upper = (uint8_t)u8_temp & 0xF0;
      u8_upper = u8_upper >> 4;
      u8_lower = (uint8_t)u8_temp & 0x0F;
    }
    else{
      u16_data = (st_menu.u16_directValue[5] * 10) - LM60_OFFSET;
      u16_data = u16_data / LM60_CELSIUS_CONSTANT;
      u8_upper = (uint8_t)u16_data & 0xF0;
      u8_upper = u8_upper >> 4;
      u8_lower = (uint8_t)u16_data & 0x0F;
      // next convert it to a hex decimal representation
      u8_temp = __u8_hexToTemp(u8_upper,u8_lower, b_changeFormat);
      // so the slider code does not have to be changed use this
      u8_slider = __u8_hexToTemp(u8_upper,u8_lower, FALSE);
      u8_upper = (uint8_t)u8_temp & 0xF0;
      u8_upper = u8_upper >> 4;
      u8_lower = (uint8_t)u8_temp & 0x0F;
    }

      esos_lcd_setCursor(ROW_TWO, 1 ); // move cursor to upper byte position
      ESOS_TASK_WAIT_LCD_WRITE_DATA(__u8_hexToChar(u8_upper)); // write upper byte
      esos_lcd_setCursor(ROW_TWO, 2 ); // move cursor to upper byte position
      ESOS_TASK_WAIT_LCD_WRITE_DATA(__u8_hexToChar(u8_lower)); // write upper byte
      esos_lcd_setCursor(ROW_TWO, 3 ); // move cursor to upper byte position
      if(b_changeFormat) {
        ESOS_TASK_WAIT_LCD_WRITE_DATA('F');
      }
      else {
        ESOS_TASK_WAIT_LCD_WRITE_DATA('C');
      }
      // ********** S L I D E R *******************************
      if(u8_oldtemp != u8_slider) {
        // the temp is in a hex representation of C
        // so need to compare the hex value
        // 27C == 0x27 (this is for LCD printing purpose)
        if(u8_slider >= 0x26) {
          // if >= 27 row two will be full
          esos_lcd_setCursor(ROW_TWO, 8 );
          ESOS_TASK_WAIT_LCD_WRITE_DATA(0xFF);
          esos_lcd_setCursor(ROW_ONE, 8 );
          ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
          esos_lcd_setCursor(ROW_ONE, 8 );
          // check to see what needs to be put into row one
          if(u8_slider >= 0x34) {
            ESOS_TASK_WAIT_LCD_WRITE_DATA(0xFF);
          }
          else if(u8_slider >= 0x32) {
            ESOS_TASK_WAIT_LCD_WRITE_DATA(0x03);
          }
          else if(u8_slider >= 0x30) {
            ESOS_TASK_WAIT_LCD_WRITE_DATA(0x04);
          }
          else if(u8_slider >= 0x28) {
            ESOS_TASK_WAIT_LCD_WRITE_DATA(0x05);
          }
        }
        else {
          // if < 27 row one will be blank
          esos_lcd_setCursor(ROW_ONE, 8 );
          ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
          esos_lcd_setCursor(ROW_TWO, 8 );
          // check to see what needs to be put into row two
          if(u8_slider <= 0x21) {
            ESOS_TASK_WAIT_LCD_WRITE_DATA(0x05);
          }
          else if(u8_slider <= 0x23) {
            ESOS_TASK_WAIT_LCD_WRITE_DATA(0x04);
          }
          else if(u8_slider <= 0x25) {
            ESOS_TASK_WAIT_LCD_WRITE_DATA(0x03);
          }
        }
        u8_oldtemp = u8_slider; // reset old temp
      }
      // ****************************************************
    // ADD CODE HERE FOR OTHER READINGS
    // else{
    // }
    // changes the output format for the sensor reading
    // for example C to F
    if(f15ui_isRpgTurningCW() || f15ui_isSW1Pressed() || f15ui_isSW2Pressed()) {
      b_changeFormat = !b_changeFormat;
      ESOS_TASK_F15UI_WAIT_UNTIL_SW2_RELEASED();
      ESOS_TASK_F15UI_WAIT_UNTIL_SW1_RELEASED();
    }
    if(f15ui_isSW3Pressed()) { // see if need to break out of child task
      b_stay = false;
      u8_oldtemp = 0x00;
    }
    ESOS_TASK_WAIT_TICKS(100);
  }
  b_stay = TRUE; // make sure we set this back to true so we can return if needed
  ESOS_TASK_END();
}

ESOS_USER_TASK(MENU)  {
  ESOS_TASK_BEGIN();
  static uint8_t pu8_tempLow[8] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x1F, 0x1F};
  static uint8_t pu8_tempMed[8] = {0x20, 0x20, 0x20, 0x20, 0x1F, 0x1F, 0x1F, 0x1F};
  static uint8_t pu8_tempHig[8] = {0x20, 0x20, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};
  esos_lcd_setCustomChar(0x58, pu8_tempHig);
  esos_lcd_setCustomChar(0x60, pu8_tempMed);
  esos_lcd_setCustomChar(0x68, pu8_tempLow);
  static int i8_counter = 1;
  static bool b_update = TRUE;
  static ESOS_TASK_HANDLE th_child;
  while (TRUE) {
    // ***************************************************************
    // HANDLES SCROLLING THOUGH MENU
    if(f15ui_isRpgTurningCCW() || f15ui_isSW2Pressed()) { // menu goes up
      i8_counter++;
      // if the menu item is hidden skip over it
      if(st_menu.b_titleHidden[i8_counter - 1]){
        i8_counter++;
      }
      b_update = TRUE; // update the screen
      // if counter is going over item count, don't update
      if(i8_counter > st_menu.i8_menuItems) {
        i8_counter = st_menu.i8_menuItems;
        b_update = FALSE;
      }
    }
    else if(f15ui_isRpgTurningCW() || f15ui_isSW1Pressed()) { // menu goes down
      i8_counter--;
      // if the menu item is hidden skip over it
      if(st_menu.b_titleHidden[i8_counter - 1]){
        i8_counter--;
      }
      b_update = TRUE;
      // if counter is going under item count, don't update
      if(i8_counter < 1) {
        i8_counter = 1;
        b_update = FALSE;
      }
    }
    // ***************************************************************
    // HANDLES UPDATING MENU IF SCROLLING
    if(b_update) {
      ESOS_TASK_WAIT_DISPLAY_TITLE(i8_counter);
      // if the menu is at the top display on the down arrow
      if(i8_counter == 1) {
        esos_lcd_setCursor(ROW_ONE, 8 );
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
        esos_lcd_setCursor(ROW_TWO, 8 );
        ESOS_TASK_WAIT_LCD_WRITE_DATA(DOWN);
      }
      // if the menu is at the bottom display only the up arrow
      else if(i8_counter == st_menu.i8_menuItems) {
        esos_lcd_setCursor(ROW_ONE, 8 );
        ESOS_TASK_WAIT_LCD_WRITE_DATA(UP);
        esos_lcd_setCursor(ROW_TWO, 8 );
        ESOS_TASK_WAIT_LCD_WRITE_DATA(' ');
      }
      // otherwise display them both
      else {
        esos_lcd_setCursor(ROW_ONE, 8 );
        ESOS_TASK_WAIT_LCD_WRITE_DATA(UP);
        esos_lcd_setCursor(ROW_TWO, 8 );
        ESOS_TASK_WAIT_LCD_WRITE_DATA(DOWN);
      }
      b_update = FALSE;
    }
    // ***************************************************************
    if(f15ui_isSW3Pressed()) {
      ESOS_ALLOCATE_CHILD_TASK(th_child);
      ESOS_TASK_F15UI_WAIT_UNTIL_SW3_RELEASED(); // wait until the SW3 is released until we continue
      if(st_menu.u8_titleType[i8_counter - 1] == STATIC) {
        ESOS_TASK_SPAWN_AND_WAIT(th_child, staticDisplay, i8_counter);
      }
      else if(st_menu.u8_subTitleType[i8_counter - 1] == SV2C) {
        ESOS_TASK_SPAWN_AND_WAIT(th_child, sv2c, i8_counter);
      }
      else if(st_menu.u8_subTitleType[i8_counter - 1] == SV3C) {
        ESOS_TASK_SPAWN_AND_WAIT(th_child, sv3c, i8_counter);
      }
      else if(st_menu.u8_subTitleType[i8_counter - 1] == SVDATA) {
        ESOS_TASK_SPAWN_AND_WAIT(th_child, svdata, i8_counter);
      }
      else if(st_menu.u8_titleType[i8_counter - 1] == READ) {
        ESOS_TASK_SPAWN_AND_WAIT(th_child, read, i8_counter);
      }
      b_update =
        TRUE; // make sure the menu is going to update upon coming out of child task
      ESOS_TASK_F15UI_WAIT_UNTIL_SW3_RELEASED(); // wait until the SW3 is released until we continue
    }
    ESOS_TASK_WAIT_TICKS(25);
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end upper_case()



/****************************************************************
** I N I T  F U N C T I O N
****************************************************************/
void esos_init_menu(void) {
  config_esos_f15ui();    // setup UI for heartbeat
  esos_lcd_configDisplay(); // config LCD hardware
  esos_lcd_init();      // initilize the LCD
  static uint8_t pu8_selectArrow[8] = { 0x02, 0x06, 0x0E, 0x1E, 0x0E, 0x06, 0x02, 0x20};
  static uint8_t pu8_upArrow[8] =     { 0x20, 0x04, 0x0E, 0x1f, 0x20, 0x20, 0x20, 0x20};
  static uint8_t pu8_downArrow[8] =   { 0x20, 0x20, 0x20, 0x20, 0x1f, 0x0E, 0x04, 0x20};
  esos_lcd_setCustomChar(0x40, pu8_selectArrow);
  esos_lcd_setCustomChar(0x48, pu8_upArrow);
  esos_lcd_setCustomChar(0x50, pu8_downArrow);
  esos_RegisterTask(MENU);
}