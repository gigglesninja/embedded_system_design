// TEAM APPLE!

/* ADC BITS

******************  dsPIC33EPxGP806 datasheet page 413 *********************
The 10-bit ADC configuration has the following key
features:
• Successive Approximation (SAR) Conversion

• Conversion Speeds of up to 1.1 Msps

• Up to 32 Analog Input Pins

• External Voltage Reference Input Pins

• Simultaneous Sampling of up to Four Analog
Input Pins

• Automatic Channel Scan mode

• Selectable Conversion Trigger Source

• Selectable Buffer Fill modes

• Four Result Alignment Options (signed/unsigned,
fractional/integer)

• Operation during CPU Sleep and Idle modes
--------------------------------------------------------------
The 12-bit ADC configuration supports all the above
features, except:
• In the 12-bit configuration, conversion speeds of up to 500 ksps are supported
• There is only one S&H amplifier in the 12-bit configuration, so simultaneous sampling of
multiple channels is not supported. Depending on the particular device pinout, the ADC
can have up to 32 analog input pins, designated AN0 through AN31. In addition, there are two analog input
pins for external voltage reference connections. These voltage reference inputs can be shared with other analog
input pins. The actual number of analog input pins and external voltage reference input configuration
depends on the specific device.

bit 15 ADON: ADC Operating Mode bit
	1 = ADC module is operating
	0 = ADC is off

bit 14 Unimplemented: Read as ‘0’

bit 13 ADSIDL: ADC Stop in Idle Mode bit
	1 = Discontinues module operation when device enters Idle mode
	0 = Continues module operation in Idle mode

bit 12 ADDMABM: DMA Buffer Build Mode bit
	1 = DMA buffers are written in the order of conversion; the module provides an address to the DMA
		channel that is the same as the address used for the non-DMA stand-alone buffer
	0 = DMA buffers are written in Scatter/Gather mode; the module provides a Scatter/Gather address
		to the DMA channel, based on the index of the analog input and the size of the DMA buffer
		bit 11 Unimplemented: Read as ‘0’

bit 10 AD12B: ADC 10-Bit or 12-Bit Operation Mode bit
	1 = 12-bit, 1-channel ADC operation
	0 = 10-bit, 4-channel ADC operation

bit 9-8 FORM<1:0>: Data Output Format bits
	For 10-Bit Operation:
		11 = Signed fractional (DOUT = sddd dddd dd00 0000, where s = .NOT.d<9>)
		10 = Fractional (DOUT = dddd dddd dd00 0000)
		01 = Signed integer (DOUT = ssss sssd dddd dddd, where s = .NOT.d<9>)
		00 = Integer (DOUT = 0000 00dd dddd dddd)

	For 12-Bit Operation:
		11 = Signed fractional (DOUT = sddd dddd dddd 0000, where s = .NOT.d<11>)
	10 = Fractional (DOUT = dddd dddd dddd 0000)
	01 = Signed Integer (DOUT = ssss sddd dddd dddd, where s = .NOT.d<11>)
	00 = Integer (DOUT = 0000 dddd dddd dddd)

bit 7-5 SSRC<2:0>: Sample Clock Source Select bits

bit 4 SSRCG: Sample Clock Source Group bit
	(See bits<7-5> for details.)

bit 3 SIMSAM: Simultaneous Sample Select bit (only applicable when CHPS<1:0> = 01 or 1x)

bit 2 ASAM: ADC Sample Auto-Start bit(3)
	1 = Sampling begins immediately after the last conversion; SAMP bit is auto-set
	0 = Sampling begins when the SAMP bit is set

bit 1 SAMP: ADC Sample Enable bit
	1 = ADC S&H amplifiers are sampling
	0 = ADC S&H amplifiers are holding

bit 0 DONE: ADC Conversion Status bit
	1 = ADC conversion cycle is completed.
	0 = ADC conversion has not started or is in progress
Automatically set by hardware when ADC conversion is complete. Software can write ‘0’ to clear the
DONE status (software not allowed to write ‘1’). Clearing this bit does NOT affect any operation in
progress. Automatically cleared by hardware at the start of a new conversion. Do not clear the DONE
bit in software if ADC Sample Auto-Start is enabled (ASAM = 1). dsPIC33EPXXX(GP/MC/MU)806/810/81
and PIC24EPXXX(GP/GU)810/814 DS70616G-page 418  2009-2012 Microchip Technology Inc.

*/
#ifndef _ESOS_PIC24_SENSOR_H
#define _ESOS_PIC24_SENSOR_H

// I N C L U D E S **********************************************************/
#include "../../pic24lib_all/esos/include/esos.h"
#include "../../pic24lib_all/esos/include/pic24/esos_pic24.h"
#include "revF15.h"
#include <stdbool.h>

// /* S T R U C T U R E S ******************************************************/
// //this structure contains the vairaibles that will be returned when a queary
// //is sent to the UI
typedef struct _st_adc_config {
  bool b_use12bits; // for easy access to the user to set 12bit or 10 bit ADC functions
  uint16_t u16_adcRes; // the resolution of the adc sample
  uint8_t u8_vRef; // hold the vRef for formating
  uint16_t u16_numSample; // the number of samples to take
  bool b_avg; // if bit is set return the average of u8_numSample
  bool b_min; // if bit is set return the minimum of u8_numSample
  bool b_max; // if bit is set return the maximum of u8_numSample
  bool b_med; // if bit is set return the median of u8_numSample
} st_adc_config_values;

/* D E F I N I T I O N S ****************************************************/

// the current channel the ADC is converting
#define ESOS_SENSOR_CH00 0x00
#define ESOS_SENSOR_CH01 0x01
#define ESOS_SENSOR_CH02 0x02
#define ESOS_SENSOR_CH03 0x03
#define ESOS_SENSOR_CH04 0x04
#define ESOS_SENSOR_CH05 0x05
#define ESOS_SENSOR_CH06 0x06
#define ESOS_SENSOR_CH07 0x07
#define ESOS_SENSOR_CH08 0x08
#define ESOS_SENSOR_CH09 0x09
#define ESOS_SENSOR_CH10 0x0A
#define ESOS_SENSOR_CH11 0x0B
#define ESOS_SENSOR_CH12 0x0C
#define ESOS_SENSOR_CH13 0x0D
#define ESOS_SENSOR_CH14 0x0E
#define ESOS_SENSOR_CH15 0x0F

// UPPER voltage referance for ADC
// The F14 dsPIC33EP512GP806 board is only capable of VREF 3.3V and 3.0V
#define ESOS_SENSOR_VREF_5V0 	0x08			// VREF = 5.0V
#define ESOS_SENSOR_VREF_4V096 	0x07			// VREF = 4.96V
#define ESOS_SENSOR_VREF_4V0 	0x06			// VREF = 4.0V
#define ESOS_SENSOR_VREF_3V3 	0x05			// VREF = 3.3V
#define ESOS_SENSOR_VREF_3V0 	0x04			// VREF = 3.0V
#define ESOS_SENSOR_VREF_2V048 	0x03			// VREF = 2.48V
#define ESOS_SENSOR_VREF_2V0 	0x02			// VREF = 2.0V
#define ESOS_SENSOR_VREF_1V024 	0x01			// VREF = 1.24V
#define ESOS_SENSOR_VREF_1V0 	0x00			// VREF = 1.0V

// Format constants for converting HEX to 16 bit variables
#define ESOS_SENSOR_FORMAT_BITS 	0x00 		// ADC returns RAW bits
#define ESOS_SENSOR_FORMAT_VOLTAGE 	0x80 		// ADC returns value as an integer representing the number of 0.1mV
#define ESOS_SENSOR_FORMAT_PERCENT 	0x40		// ADC returns value as integer percentage 0%-100% scale

#define ESOS_SENSOR_ONE_SHOT 0x00 // the sensor child task will return the single ADC conversion result, just like the quick read function

#define ESOS_SENSOR_AVG2 	0x01  // sensor read child task will return the arithmetic average of the last 2 sequential ADC readings
#define ESOS_SENSOR_AVG4 	0x02  // sensor read child task will return the arithmetic average of the last 4 sequential ADC readings
#define ESOS_SENSOR_AVG8 	0x03  // sensor read child task will return the arithmetic average of the last 8 sequential ADC readings
#define ESOS_SENSOR_AVG16 	0x04  // sensor read child task will return the arithmetic average of the last 16 sequential ADC readings
#define ESOS_SENSOR_AVG32 	0x05  // sensor read child task will return the arithmetic average of the last 32 sequential ADC readings
#define ESOS_SENSOR_AVG64 	0x06  // sensor read child task will return the arithmetic average of the last 64 sequential ADC readings

#define ESOS_SENSOR_MIN2 	0x21  // sensor read child task will return the minimum of the most recent 2 sequential ADC readings
#define ESOS_SENSOR_MIN4 	0x22  // sensor read child task will return the minimum of the most recent 4 sequential ADC readings
#define ESOS_SENSOR_MIN8 	0x23  // sensor read child task will return the minimum of the most recent 8 sequential ADC readings
#define ESOS_SENSOR_MIN16 	0x24  // sensor read child task will return the minimum of the most recent 16 sequential ADC readings
#define ESOS_SENSOR_MIN32 	0x25  // sensor read child task will return the minimum of the most recent 32 sequential ADC readings
#define ESOS_SENSOR_MIN64 	0x26  // sensor read child task will return the minimum of the most recent 64 sequential ADC readings

#define ESOS_SENSOR_MAX2 	0x41  // sensor read child task will return the maximum of the most recent 2 sequential ADC readings
#define ESOS_SENSOR_MAX4 	0x42  // sensor read child task will return the maximum of the most recent 4 sequential ADC readings
#define ESOS_SENSOR_MAX8 	0x43  // sensor read child task will return the maximum of the most recent 8 sequential ADC readings
#define ESOS_SENSOR_MAX16 	0x44  // sensor read child task will return the maximum of the most recent 16 sequential ADC readings
#define ESOS_SENSOR_MAX32 	0x45  // sensor read child task will return the maximum of the most recent 32 sequential ADC readings
#define ESOS_SENSOR_MAX64 	0x46  // sensor read child task will return the maximum of the most recent 64 sequential ADC readings

#define ESOS_SENSOR_MEDIAN2 0x81  // sensor read child task will return the median value of the most recent 2 sequential ADC readings
#define ESOS_SENSOR_MEDIAN4 0x82  // sensor read child task will return the median value of the most recent 4 sequential ADC readings
#define ESOS_SENSOR_MEDIAN8 0x83  // sensor read child task will return the median value of the most recent 8 sequential ADC readings
#define ESOS_SENSOR_MEDIAN16 0x84 // sensor read child task will return the median value of the most recent 16 sequential ADC readings
#define ESOS_SENSOR_MEDIAN32 0x85 // sensor read child task will return the median value of the most recent 32 sequential ADC readings
#define ESOS_SENSOR_MEDIAN64 0x86 // sensor read child task will return the median value of the most recent 64 sequential ADC readings

#define ESOS_USE_ADC_12_BIT(x)	(__set_adc_12_bit(x)); // allow the user to select 12 bit conversion

/* E X T E R N S ************************************************************/
extern struct stTask    __stChildTaskADC;

/* M A C R O S **************************************************************/
#define ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(CHCONST,VREFCONST)   					\
do {                                                            					\
        ESOS_TASK_WAIT_WHILE(__esos_IsSystemFlagSet(__ESOS_SYS_ADC_IS_BUSY));       \
        __esos_SetSystemFlag(__ESOS_SYS_ADC_IS_BUSY);                               \
        __esos_pic24_config_ADC1(CHCONST, VREFCONST);                                          \
} while (0)

// Task reads the ADC sample ONE time and returns to user
#define ESOS_TASK_WAIT_SENSOR_QUICK_READ(u16_data)                             	    \
        ESOS_TASK_SPAWN_AND_WAIT((ESOS_TASK_HANDLE)&__stChildTaskADC, __esos_pic24_readADC, (&u16_data))

/// Task reads the ADC sample over 1-64 samples, processes, and formats based on PROCESSCONST and FMTCONST
#define ESOS_TASK_WAIT_SENSOR_READ(u16_data, PROCESSCONST, FMTCONST)      			\
        ESOS_TASK_SPAWN_AND_WAIT( (ESOS_TASK_HANDLE)&__stChildTaskADC, __esos_pic24_readSensor, (&u16_data), PROCESSCONST, FMTCONST)


/*** F U N C T I O N S  ****************************************************/

bool ESOS_SENSOR_CLOSE(void);
void __esos_pic24_config_ADC1(uint8_t u8_channel, uint8_t u8_vRef);
void __set_adc_12_bit(bool b_use12bits);
uint32_t __esos_pic24_get_vref(void);

/* C H I L D T A S K - P R O T O T Y P E S ********************************/

ESOS_CHILD_TASK(__esos_pic24_readADC, uint16_t *u16_data);
ESOS_CHILD_TASK(__esos_pic24_readSensor, uint16_t *u16_data, uint8_t u8_process_mask, uint8_t u8_format_mask);

#endif // end ESOS_PIC24_SENSOR_H