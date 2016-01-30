// Team Apple
#define BUFFER_SIZE 60

#define ASCII_SPACE     0x20
#define ASCII_0         0x30
#define ASCII_1         0x31
#define ASCII_2         0x32
#define ASCII_3         0x33
#define ASCII_4         0x34
#define ASCII_5         0x35
#define ASCII_6         0x36
#define ASCII_7         0x37
#define ASCII_8         0x38
#define ASCII_9         0x39
#define ASCII_COLON     0x3A
#define ASCII_A         0x41
#define ASCII_B         0x42
#define ASCII_C         0x43
#define ASCII_D         0x44
#define ASCII_E         0x45
#define ASCII_F         0x46
#define ASCII_G         0x47
#define ASCII_H         0x48
#define ASCII_I         0x49
#define ASCII_J         0x4a
#define ASCII_K         0x4b
#define ASCII_L         0x4c
#define ASCII_M         0x4d
#define ASCII_N         0x4e
#define ASCII_O         0x4f
#define ASCII_P         0x50
#define ASCII_Q         0x51
#define ASCII_R         0x52
#define ASCII_S         0x53
#define ASCII_T         0x54
#define ASCII_U         0x55
#define ASCII_V         0x56
#define ASCII_W         0x57
#define ASCII_X         0x58
#define ASCII_Y         0x59
#define ASCII_Z         0x5a

uint8_t au8_buffer[BUFFER_SIZE];
bool b_use_buffer;
uint8_t u8_buffer_head;




#define ESOS_TASK_WAIT_ON_COMMAND(u8_int) \
do {\
        ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();\
        ESOS_TASK_WAIT_ON_GET_UINT8(u8_int);\
        ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();\
} while(u8_int == ASCII_SPACE)



ESOS_USER_TASK(task_name_TBA) {
    static uint8_t      u8_char;
    
    ESOS_TASK_BEGIN();
    while(TRUE) {
        
    } //endof while(TRUE)
    
} // end 









// User Init

void user_init(void) {
    esos_RegisterTask(task_name_TBA);
} //end user_init())