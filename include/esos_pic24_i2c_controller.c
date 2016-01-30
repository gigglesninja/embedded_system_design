// TEAM APPLE!

/*** I N C L U D E S **********************************************************/
#include "esos_pic24_i2c_controller.h"
#include "../../../pic24lib_all/esos/include/pic24/esos_pic24_i2c.h"
#include <stdbool.h>
#include "esos_lcd_menu.h"

#define DS1631ADDR 0x90   //DS1631 address with all pins tied low
#define ACCESS_CONFIG 0xAC
#define CONFIG_COMMAND 0x0C //continuous conversion, 12-bit mode
#define START_CONVERT 0x51
#define READ_TEMP 0xAA

ESOS_SEMAPHORE(sem_readyToSend);
ESOS_SEMAPHORE(sem_dataSent);
ESOS_SEMAPHORE(sem_fastRead);
ESOS_SEMAPHORE(sem_fastWrite);
// ESOS_SIGNAL_SEMAPHORE(sem_readyToSend, 1);
// ESOS_TASK_WAIT_SEMAPHORE(sem_readyToSend, 1);
// ESOS_INIT_SEMAPHORE(sem_readyToSend, 0);

typedef struct i2cBuffer {
  uint8_t u8_i2cBuffer[64];
  uint8_t u8_slaveAddress;
  uint8_t u8_readBytes;
  int16_t i_position;
  int16_t i_bufferDepth;
  bool b_readytoSend;
} st_buffer;

st_buffer buffer;

uint8_t uint8_to_hex(uint8_t i) {
  if(i == '0') {
    return 0x00;
  }
  else if(i == '1') {
    return 0x01;
  }
  else if(i == '2') {
    return 0x02;
  }
  else if(i == '3') {
    return 0x03;
  }
  else if(i == '4') {
    return 0x04;
  }
  else if(i == '5') {
    return 0x05;
  }
  else if(i == '6') {
    return 0x06;
  }
  else if(i == '7') {
    return 0x07;
  }
  else if(i == '8') {
    return 0x08;
  }
  else if(i == '9') {
    return 0x09;
  }
  else if(i == 'A') {
    return 0x0A;
  }
  else if(i == 'B') {
    return 0x0B;
  }
  else if(i == 'C') {
    return 0x0C;
  }
  else if(i == 'D') {
    return 0x0D;
  }
  else if(i == 'E') {
    return 0x0E;
  }
  else if(i == 'F') {
    return 0x0F;
  }
  return 0xFF;
}

ESOS_USER_TASK(I2C_CONTROLLER) {
  ESOS_TASK_BEGIN();
  static uint8_t u8_choice;
  static uint8_t u8_hi, u8_lo;
  static int8_t i_formatedBuffer;
  static ESOS_TASK_HANDLE th_child;
  static bool startSend;
  static bool formating;
  static bool fastRead;
  formating = FALSE;
  startSend = FALSE;
  fastRead = FALSE;
  i_formatedBuffer = 0;
  while (TRUE) {
    // get info for the buffer
    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    ESOS_TASK_WAIT_ON_GET_UINT8(u8_choice);
    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
    // store incoming options until P is written
    if(u8_choice != 'P') {
      // need to make sure we don't take charactors we don't need
      if(u8_choice == 'R') {
        ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
        ESOS_TASK_WAIT_ON_GET_UINT8(u8_choice); // we don't care about this one
        ESOS_TASK_WAIT_ON_GET_UINT8(u8_choice);
        ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
        // if two or more just read 2
        if(uint8_to_hex(u8_choice) >= 0x02) {
          buffer.u8_readBytes = 0x02;
        }
        // if only one read one
        else {
          buffer.u8_readBytes = 0x01;
        }
        // if the buffer postion is not 0, we need to clear the buffer before sending
        if(buffer.i_position ) {
          formating = TRUE;
          buffer.i_bufferDepth = buffer.i_position;
          buffer.i_position = 0;
          fastRead = TRUE;
          // wait before this task can run again
        }
      }
      else if(u8_choice == 'W'){
      	ESOS_SIGNAL_SEMAPHORE(sem_fastWrite, 1);
      	ESOS_TASK_WAIT_SEMAPHORE(sem_dataSent, 1);
      }
      else if(uint8_to_hex(u8_choice) != 0xFF) {
        // if the hex char is good put it in the buffer
        buffer.u8_i2cBuffer[buffer.i_position] = u8_choice;
        buffer.i_position++;
      }
    }
    // we are ready to format and send
    else if(u8_choice == 'P') {
      formating = TRUE;
      buffer.i_bufferDepth = buffer.i_position;
      buffer.i_position = 0;
    }
    // format before sending since we can only accept 8 bit chars
    // 8 bit chars must be sent by using MSB and LSB, for instance
    // A should be sent as 0A
    while(formating) {
      u8_hi = buffer.u8_i2cBuffer[buffer.i_position];// get first char
      u8_lo = buffer.u8_i2cBuffer[++buffer.i_position]; // get next char
      u8_hi = uint8_to_hex(u8_hi);
      u8_lo = uint8_to_hex(u8_lo);
      // store in new buffer
      buffer.u8_i2cBuffer[i_formatedBuffer++] = u8_hi << 4 | u8_lo;
      buffer.i_position++; // need to increment 1 more time
      if(buffer.i_position >= buffer.i_bufferDepth) {
        formating = FALSE; // formating is finished
        startSend = TRUE; // start sending
        buffer.i_bufferDepth = i_formatedBuffer;
        buffer.i_position = 1;
        i_formatedBuffer = 0;
      }
    }
    if(startSend) {
      // signal send
      ESOS_SIGNAL_SEMAPHORE(sem_readyToSend, 1);
      // wait before this task can run again
      ESOS_TASK_WAIT_SEMAPHORE(sem_dataSent, 1);
      if(fastRead) {
        ESOS_SIGNAL_SEMAPHORE(sem_fastRead, 1);
        ESOS_TASK_WAIT_SEMAPHORE(sem_dataSent, 1);
        fastRead = FALSE;
      }
      startSend = FALSE;
    }
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end upper_case()

ESOS_USER_TASK(FAST_READ) {
  ESOS_TASK_BEGIN();
  static uint8_t u8_hi, u8_lo;
  while(TRUE) {
    ESOS_TASK_WAIT_SEMAPHORE(sem_fastRead, 1);
    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    if(buffer.u8_readBytes == 0x01) {
      ESOS_TASK_WAIT_ON_READ1I2C1(buffer.u8_slaveAddress, u8_hi);
      ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_hi);
    }
    else {
      ESOS_TASK_WAIT_ON_READ2I2C1(buffer.u8_slaveAddress, u8_hi, u8_lo);
      ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_hi);
      ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_lo);
    }
    ESOS_TASK_WAIT_ON_SEND_STRING("\n\r");
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    ESOS_SIGNAL_SEMAPHORE(sem_dataSent, 1);
  }
  ESOS_TASK_END();
}

ESOS_USER_TASK(FAST_WRITE) {
  ESOS_TASK_BEGIN();
  static uint8_t u8_hi, u8_lo;
  while(TRUE) {
    ESOS_TASK_WAIT_SEMAPHORE(sem_fastWrite, 1);
    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    ESOS_TASK_WAIT_ON_GET_UINT8(u8_hi);
    ESOS_TASK_WAIT_ON_GET_UINT8(u8_lo);
    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
    u8_hi = uint8_to_hex(u8_hi);
    u8_lo = uint8_to_hex(u8_lo);
    ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
    ESOS_TASK_WAIT_ON_WRITE1I2C1(buffer.u8_slaveAddress, u8_hi << 4 | u8_lo);
    ESOS_TASK_SIGNAL_AVAILABLE_I2C();
    ESOS_SIGNAL_SEMAPHORE(sem_dataSent, 1);
  }
  ESOS_TASK_END();
}

ESOS_USER_TASK(SEND_DATA) {
  ESOS_TASK_BEGIN();
  static bool read;
  static uint8_t u8_hi, u8_lo;
  while(TRUE) {
    read = TRUE;
    // wait until we are ready to send
    ESOS_TASK_WAIT_SEMAPHORE(sem_readyToSend, 1);
    // save the slave address for quick read and writes
    buffer.u8_slaveAddress = buffer.u8_i2cBuffer[0];
    while(read) {
      ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
      // write the info from the buffer
      ESOS_TASK_WAIT_ON_WRITE1I2C1(buffer.u8_slaveAddress,
                                   buffer.u8_i2cBuffer[buffer.i_position++]);
      // check to see if we need to read from the I2C R/W = 1
      if(buffer.u8_slaveAddress & 0x01) {
        // read from the buffer then write them to the UART
        ESOS_TASK_WAIT_ON_READ2I2C1(buffer.u8_slaveAddress, u8_hi, u8_lo);
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_hi);
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_lo);
        ESOS_TASK_WAIT_ON_SEND_STRING("\n\r");
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
      }
      // release and turn off the I2C bus
      ESOS_TASK_SIGNAL_AVAILABLE_I2C();
      if(buffer.i_position >= buffer.i_bufferDepth) {
        read = FALSE;
        buffer.i_position = 0;
        buffer.b_readytoSend = TRUE;
        ESOS_SIGNAL_SEMAPHORE(sem_dataSent, 1);
      }
    }
  }
  ESOS_TASK_END();
}
void init_i2c_task(void) {
  buffer.i_position = 0;
  buffer.b_readytoSend = FALSE;
  buffer.i_bufferDepth = 0;
  esos_pic24_configI2C1(400);
  esos_RegisterTask(I2C_CONTROLLER);
  esos_RegisterTask(SEND_DATA);
  esos_RegisterTask(FAST_READ);
  esos_RegisterTask(FAST_WRITE);
  ESOS_INIT_SEMAPHORE(sem_readyToSend, 0);
  ESOS_INIT_SEMAPHORE(sem_dataSent, 0);
  ESOS_INIT_SEMAPHORE(sem_fastRead, 0);
  ESOS_INIT_SEMAPHORE(sem_fastWrite, 0);
}