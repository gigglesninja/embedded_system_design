#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../include/esos_f15_ui.h"
#include "../../include/esos_pic24_sensor.h"
#include "../../include/esos_pic24_lcd.h"


ESOS_USER_TASK(TEST)  {
  ESOS_TASK_BEGIN();
  ESOS_TASK_WAIT_LCD_SET_CG_ADDRESS(0x40);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0b10000);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0b10000);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0b10000);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0b10000);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0b10000);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0b10000);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0b10000);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0b10000);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0b10000);
  ESOS_TASK_WAIT_TICKS(200);
  esos_lcd_setCursor(ROW_ONE, 1);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0x00);
  esos_lcd_setCursor(ROW_ONE, 2);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0x01);
  esos_lcd_setCursor(ROW_ONE, 3);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0x02);
  esos_lcd_setCursor(ROW_ONE, 4);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0x03);
  esos_lcd_setCursor(ROW_ONE, 5);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0x04);
  esos_lcd_setCursor(ROW_ONE, 5);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0x05);
  esos_lcd_setCursor(ROW_ONE, 7);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0x06);
  esos_lcd_setCursor(ROW_ONE, 8);
  ESOS_TASK_WAIT_LCD_WRITE_DATA(0x07);
  ESOS_TASK_END();
} // end upper_case()

void user_init(void) {
  config_esos_f15ui();		// setup UI for heartbeat
  f15ui_flashLED3(500);
  esos_lcd_configDisplay(); // config LCD hardware
  esos_lcd_init();			// initilize the LCD
  // user_init() should register at least one user task
  esos_RegisterTask(TEST);


} // end user_init()