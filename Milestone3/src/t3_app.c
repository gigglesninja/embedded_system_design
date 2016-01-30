
#include "../../../pic24lib_all/esos/include/esos.h"
#include "../../include/esos_f15_ui.h"


// Globals
char psz_SW1pressed[] = "SW1 pressed\n";
char psz_SW1doublePressed[] = "SW1 double pressed\n";
char psz_SW1released[] = "SW1 released";

char psz_SW2pressed[] = "SW2 pressed\n";
char psz_SW2doublePressed[] = "SW2 double pressed\n";
char psz_SW2released[] = "SW2 released\n";

char psz_SW3pressed[] = "SW3 pressed\n";
char psz_SW3doublePressed[] = "SW3 double pressed\n";
char psz_SW3released[] = "SW3 released\n";

char psz_Menu[] =   "                   \n---Welcome to F15 APPLE!---\n\
                    To view this menu select m\n\
                    To set switch 1 double press period select 1\n\
                    To set switch 2 double press period select 2\n\
                    To set switch 3 double press period select 3\n\
                    To select RPG thresholds select 4\n";

char psz_buttonOptions[] = "Option          Period\n\
1           100ms\n\
2           250ms\n\
3           500ms\n\
4           1000ms\n";

char psz_RPGOptions[] = "Option                 Speeds\n\
1           medium 2 turns per 500ms, fast 4 turns per 500ms\n\
2           medium 4 turns per 500ms, fast 8 turns per 500ms\n\
3           medium 6 turns per 500ms, fast 12 turns per 500ms\n\
4           medium 8 turns per 500ms, fast 16 turns per 500ms\n";

char psz_prompt[] = "\nWating on an option, to print the menu select m \n";


// displays a menu for the user to experiment with values for switch double press and RPG theresholds
ESOS_USER_TASK(serial_menu) {
    ESOS_TASK_BEGIN();

    //variables to hold the periods
    static uint32_t SW1DPPeriod;
    static uint32_t SW2DPPeriod;
    static uint32_t SW3DPPeriod;
    static uint16_t RPGMedium;
    static uint16_t RPGFast;

    //to keep track of the user response
    static  uint8_t u8_choice;

    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_Menu);
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    while (1) {
        //print prompt
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_prompt);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        //wait until a option is chosen
        ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
        ESOS_TASK_WAIT_ON_GET_UINT8(u8_choice);
        ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
        //determain what was chosen
        if(u8_choice == 'm'){
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(psz_Menu);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        else if(u8_choice == '1'){
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(psz_buttonOptions);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_choice);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
            if(u8_choice == '1'){
                SW1DPPeriod = 100;
            }
            else if(u8_choice == '2'){
                SW1DPPeriod = 250;
            }
            else if(u8_choice == '3'){
                SW1DPPeriod = 500;
            }
            else{
                SW1DPPeriod = 1000;
            }
            f15ui_setSW1DPPeriod(SW1DPPeriod);
        }
        else if(u8_choice == '2'){
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(psz_buttonOptions);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_choice);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
            if(u8_choice == '1'){
                SW2DPPeriod = 100;
            }
            else if(u8_choice == '2'){
                SW2DPPeriod = 250;
            }
            else if(u8_choice == '3'){
                SW2DPPeriod = 500;
            }
            else{
                SW2DPPeriod = 1000;
            }
            f15ui_setSW2DPPeriod(SW2DPPeriod);
        }
        else if(u8_choice == '3'){
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(psz_buttonOptions);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_choice);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
            if(u8_choice == '1'){
                SW3DPPeriod = 100;
            }
            else if(u8_choice == '2'){
                SW3DPPeriod = 250;
            }
            else if(u8_choice == '3'){
                SW3DPPeriod = 500;
            }
            else{
                SW3DPPeriod = 1000;
            }
            f15ui_setSW3DPPeriod(SW3DPPeriod);
        }
        else if(u8_choice == '4'){
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(psz_RPGOptions);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_choice);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
            if(u8_choice == '1'){
                RPGFast = 4;
                RPGMedium = 2 ;
            }
            else if(u8_choice == '2'){
                RPGFast = 8;
                RPGMedium = 4 ;
            }
            else if(u8_choice == '3'){
                RPGFast = 12;
                RPGMedium = 6 ;
            }
            else{
                RPGFast = 16;
                RPGMedium = 8 ;
            }
            f15ui_setRPGFast(RPGFast);
            f15ui_setRPGMedium(RPGMedium);
        }
    }
    ESOS_TASK_END();
}

// LED 1 is controlled by SW1 SW2 and SW3.
// If SW3 is not pressed SW1 lights LED1 on press
// If SW# is pressed SW2 lights LED1 on press
ESOS_USER_TASK(switch_detection_LED1) {
    ESOS_TASK_BEGIN();
    while (TRUE){
        //if sw3 released sw1 controls led 1
        if(f15ui_isSW3Released()) {
            if(f15ui_isSW1Pressed()){
                f15ui_turnLED1ON();
            } else {
                f15ui_turnLED1OFF();
            }
            if(f15ui_isSW1DoublePressed()){
            //could write a child task here later to clean this up
            f15ui_turnLED1ON();
            ESOS_TASK_WAIT_TICKS(250);
            f15ui_turnLED1OFF();
            ESOS_TASK_WAIT_TICKS(250);

            f15ui_turnLED1ON();
            ESOS_TASK_WAIT_TICKS(250);
            f15ui_turnLED1OFF();
            ESOS_TASK_WAIT_TICKS(250);

            f15ui_turnLED1ON();
            ESOS_TASK_WAIT_TICKS(250);
            f15ui_turnLED1OFF();
            ESOS_TASK_WAIT_TICKS(250);
            }
        } 
        //if sw3 pressed sw1 controls led1
        if(f15ui_isSW3Pressed()){
            if(f15ui_isSW2Pressed()){
                f15ui_turnLED1ON();
            } else {
                f15ui_turnLED1OFF();
            }
            if(f15ui_isSW2DoublePressed()){
                //could write a child task here later to clean this up
                f15ui_turnLED1ON();
                ESOS_TASK_WAIT_TICKS(250);
                f15ui_turnLED1OFF();
                ESOS_TASK_WAIT_TICKS(250);

                f15ui_turnLED1ON();
                ESOS_TASK_WAIT_TICKS(250);
                f15ui_turnLED1OFF();
                ESOS_TASK_WAIT_TICKS(250);

                f15ui_turnLED1ON();
                ESOS_TASK_WAIT_TICKS(250);
                f15ui_turnLED1OFF();
                ESOS_TASK_WAIT_TICKS(250);
            }
        }
        
        ESOS_TASK_WAIT_TICKS(10);
    }
    ESOS_TASK_END();
}

// task controls serial output for SW1
// task blocks until SW1 is pressed, then 
// blocks until SW1 is released 
ESOS_USER_TASK(SW1_to_serial) {
    ESOS_TASK_BEGIN();
    while (TRUE){
        ESOS_TASK_F15UI_WAIT_UNTIL_SW1_PRESSED();
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_SW1pressed);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        ESOS_TASK_F15UI_WAIT_UNTIL_SW1_RELEASED();
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_SW1released);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    }
    ESOS_TASK_END();
}

// task controls serial output for SW1
// task blocks until SW2 is pressed, then 
// blocks until SW2 is released 
ESOS_USER_TASK(SW2_to_serial) {
    ESOS_TASK_BEGIN();
    while (TRUE){
        ESOS_TASK_F15UI_WAIT_UNTIL_SW2_PRESSED();
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_SW2pressed);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        ESOS_TASK_F15UI_WAIT_UNTIL_SW2_RELEASED();
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_SW2released);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    }
    ESOS_TASK_END();
}

// task controls serial output for SW1
// task blocks until SW3 is pressed, then 
// blocks until SW3 is released  
ESOS_USER_TASK(SW3_to_serial) {
    ESOS_TASK_BEGIN();
    while (TRUE){
        ESOS_TASK_F15UI_WAIT_UNTIL_SW3_PRESSED();
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_SW3pressed);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        ESOS_TASK_F15UI_WAIT_UNTIL_SW3_RELEASED();
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_SW3released);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    }
    ESOS_TASK_END();
}

// prints the direction of the RPG if it is currently turnning
ESOS_USER_TASK(RPG_to_serial) {
    ESOS_TASK_BEGIN();
    
    while (TRUE){
        if(f15ui_isRpgTurningCW()){
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING("RPG CW\n");
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        else if(f15ui_isRpgTurningCCW()){
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING("RPG CCW\n");
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        ESOS_TASK_WAIT_TICKS(10); // task needs to run at 10 ms to read values correctly
    }
    ESOS_TASK_END();
}

// RPG controls LED2. If RPG is turning slow LED2 is ON.
// IF RPG is turning medium RPG blinks at 500ms
// IF RPG is turning fast RPG blinks at 100ms
ESOS_USER_TASK(RPG_medium_detection_LED2) {
    ESOS_TASK_BEGIN();
    while (TRUE) {
    	ESOS_TASK_F15UI_WAIT_UNTIL_RPG_TURNS_MEDIUM();
    	f15ui_toggleLED2();
        ESOS_TASK_WAIT_TICKS(500);
    }
    ESOS_TASK_END();
}

// RPG controls LED2. If RPG is turning slow LED2 is ON.
// IF RPG is turning medium RPG blinks at 500ms
// IF RPG is turning fast RPG blinks at 100ms
ESOS_USER_TASK(RPG_fast_detection_LED2) {
    ESOS_TASK_BEGIN();
    while (TRUE) {
    	ESOS_TASK_F15UI_WAIT_UNTIL_RPG_TURNS_FAST();
    	f15ui_toggleLED2();
        ESOS_TASK_WAIT_TICKS(100);
    }
    ESOS_TASK_END();
}

// RPG controls LED2. If RPG is turning slow LED2 is ON.
// IF RPG is turning medium RPG blinks at 500ms
// IF RPG is turning fast RPG blinks at 100ms
ESOS_USER_TASK(RPG_on_detection_LED2) {
	ESOS_TASK_BEGIN();
    while (TRUE) {
		if(!(f15ui_isRpgTurningMedium()) && (!(f15ui_isRpgTurningFast()))){
			if(f15ui_isRpgTurning()){
				f15ui_turnLED2ON();
				ESOS_TASK_WAIT_TICKS(1000); //YOLO
			}
			else{
				f15ui_turnLED2OFF();
			}
		}
		ESOS_TASK_WAIT_TICKS(10);
    }
    ESOS_TASK_END();
}

// Detects a double press for SW1 and prints to serial 
ESOS_USER_TASK(SW1_double_press_detection) {
    ESOS_TASK_BEGIN();
    while (TRUE){
        ESOS_TASK_F15UI_WAIT_UNTIL_SW1_DOUBLE_PRESSED();
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_SW1doublePressed);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    }
    ESOS_TASK_END();
}

// Detects a double press for SW2 and prints to serial 
ESOS_USER_TASK(SW2_double_press_detection) {
    ESOS_TASK_BEGIN();
    while (TRUE){
        ESOS_TASK_F15UI_WAIT_UNTIL_SW2_DOUBLE_PRESSED();
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_SW2doublePressed);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    }
    ESOS_TASK_END();
}

// Detects a double press for SW3 and prints to serial 
ESOS_USER_TASK(SW3_double_press_detection) {
    ESOS_TASK_BEGIN();
    while (TRUE){
        ESOS_TASK_F15UI_WAIT_UNTIL_SW3_DOUBLE_PRESSED();
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_SW3doublePressed);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    }
    ESOS_TASK_END();
}

void user_init(void) {

	// configure our hardware
    config_esos_f15ui();
    
    // register our task
    esos_RegisterTask(serial_menu);
    esos_RegisterTask(switch_detection_LED1);
    esos_RegisterTask(RPG_fast_detection_LED2);
    esos_RegisterTask(RPG_medium_detection_LED2);
    esos_RegisterTask(RPG_on_detection_LED2);
    esos_RegisterTask(SW1_to_serial);
    esos_RegisterTask(SW2_to_serial);
    esos_RegisterTask(SW3_to_serial);
    esos_RegisterTask(SW1_double_press_detection);
    esos_RegisterTask(SW2_double_press_detection);
    esos_RegisterTask(SW3_double_press_detection);
    esos_RegisterTask(RPG_to_serial);
    
    // do heartbeat
    f15ui_flashLED3(500);
}