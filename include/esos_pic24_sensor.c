// TEAM APPLE!

/*** I N C L U D E S ************************************************/
#include "esos_pic24_sensor.h"

/*** G L O B A L S **************************************************/
struct stTask    __stChildTaskADC;
volatile  st_adc_config_values
st_adcValues; // access to our user defined ADC configuration

// need to clear the flag during the user init
#define ESOS_TASK_SIGNAL_AVAILABLE_ADC()    __esos_ClearSystemFlag(__ESOS_SYS_ADC_IS_BUSY) // __ESOS_SYS_ADC_IS_BUSY defined esos.h line 470
#define IS_CONVERSION_COMPLETE_ADC1()       AD1CON1bits.DONE // Determines if the conversion on AD1 is done.
#define TURN_ADC_OFF()                      (AD1CON1bits.ADON = 0) // turns adc off
#define TURN_ADC_ON()                       (AD1CON1bits.ADON = 1) // turns adc off
#define FORMAT_12_BITS                      0x0C
#define FORMAT_10_BITS                      0x0A
#define PERCENTAGE                          0x64

/*** P R I V A T E - F U N C T I O N S*******************************/

// a compare function for our sorting algorithum for med values
int cmpfunc (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}

// sets the adc to use 12 if true, 10 bits if false.
void __set_adc_12_bit(bool b_use12bits) {
  st_adcValues.b_use12bits = b_use12bits;
}

// sets the referance for formatting. see ESOS_READ task for formula
uint16_t get_vref(void) {
  if(st_adcValues.u8_vRef == ESOS_SENSOR_VREF_1V0) {
    return 10000;
  }
  else if(st_adcValues.u8_vRef == ESOS_SENSOR_VREF_1V024) {
    return 10240;
  }
  else if (st_adcValues.u8_vRef == ESOS_SENSOR_VREF_2V0) {
    return 20000;
  }
  else if (st_adcValues.u8_vRef == ESOS_SENSOR_VREF_2V048) {
    return 20480;
  }
  else if (st_adcValues.u8_vRef == ESOS_SENSOR_VREF_3V0) {
    return 30000;
  }
  else if (st_adcValues.u8_vRef == ESOS_SENSOR_VREF_3V3) {
    return 33000;
  }
  else if (st_adcValues.u8_vRef == ESOS_SENSOR_VREF_4V0) {
    return 40000;
  }
  else if (st_adcValues.u8_vRef == ESOS_SENSOR_VREF_4V096) {
    return 40960;
  }
  else if (st_adcValues.u8_vRef == ESOS_SENSOR_VREF_5V0) {
    return 50000;
  }
  else {
    return PERCENTAGE;
  }
}

/*  PIC24 ADC CONFIGURATION
        Currently all available PIC24 family uC have only one ADC. So use AD1CONx for
        control registers.

        Macros used to assign status of registers live in pic24_adc.h

        AD1CON1 configuration register
        Bit 15 ADON: Enable ADC
        Bit 13 ADSIDL: Stop in idle mode
        Bit 10 AD12B: 1 = 12bit, 1-channel; 0 = 10 bit, multiple-channel ADC operation
        Bit 9-8 FORM: Data output format bits
        Bit 7-5 SSRC: Sample clock source select bits
        Bit 3 SIMSAM: Simultaneous sample select bit (10-bit mode only)
        Bit 2 ASAM: ADC sample auto-start bit; 1 = start immediately; 0 = start after SAMP == 1
        Bit 1 SAMP: Sample enable/status bit
        Bit 0 DONE: ADC conversion status bit; 1 = complete; 0 = in progress

        AD1CON2 configuration register
        Bit 15-13 VCFG: Converter VRef configuration.
        Bit 10 CSCNA: Input Scan Select
        Bit 9-8 CHPS: Channel Select bit(10-bit mode only)
        Bit 7 BUFS: Buffer Fill Status bit
        Bit 6-2 SMPI: Increment Rate Configuration
        Bit 1 BUFM: Buffer Fill Mode
        Bit 0 ALTS: Alternate Input Sample Mode

        AD1CON3 configuration register
        Bit 15 ADRC: ADC conversion clock select; 1 = internal ADC clock; 0 = system clock
        Bit 12-8 SAMC: Auto Sample Time Select Bits
        Bit 7-0 ADCS: ADC Conversion Clock

        AD1CHS0 Configuration register
        Bit 15 CH0NB: Select Negative input for CH0 SHA for sample B bits
        Bit 8-11 CH0SB: Select positve input for CH0 Sample B bits
        Bit 7 CH0NA: Select Negative input ofr CH0 SHA for sample A bits
        Bit 4-0 CH0NA: Select positive input ofr CHO ssample A bits
        ***** Even though CH0SB and CH0SA are 4 bit, we can only use up to
        ***** 01111 0x0F See datasheet pg 426 for explanation
*/

// configes the ADC with the channel and sets the configuration for upper and lower reference
void __esos_pic24_config_ADC1(uint8_t u8_channel, uint8_t u8_vRef) {
  TURN_ADC_OFF(); // turn off ADC (changing setting while ADON is not allowed)
  st_adcValues.u8_vRef = u8_vRef; // save our vRef incase we need to use it later
  // set the AD1CON1 control register
  // ADC_12Bit == 0x0400 A/D conversion is 12bits instead of 10bits
  // ADC_CLK_AUTO == 0x00E0 Internal counter ends sampling and starts conversion (Auto convert)
  // ADC_AUTO_SAMPLING_OFF == 0x0000 Sampling begins when SAMP bit is set
  if(st_adcValues.b_use12bits) {
    AD1CON1 = ADC_CLK_AUTO | ADC_AUTO_SAMPLING_OFF | ADC_12BIT;
    st_adcValues.u16_adcRes = 4096;
  }
  else {
    AD1CON1 = ADC_CLK_AUTO | ADC_AUTO_SAMPLING_OFF;
    st_adcValues.u16_adcRes = 1024;
  }
  // set AD1CON2 control register
  // We will only allow for External VREF+ to be set on the F14_Embedded board
  // The dsPIC33EP512GP806 uC AVdd and AVss pins are only tolerant up to 3.3V
  // ******** see page 419 data for bits ******************
  // ADC_VREF_AVDD_AVSS == 0x8000
  // POSSIBLE bits
  // 000 1xx internal 3.3
  // 001 external VREF+
  if(u8_vRef == ESOS_SENSOR_VREF_3V3) {
    AD1CON2 = ADC_VREF_AVDD_AVSS; // use internal 3.3 upper and lower
  }
  else {
    AD1CON2 = ADC_VREF_EXT_AVSS; // use external upper and internal lower
  }
  // set AD1CON3 control register
  // ADC_CONV_CLK_INTERNAL_RC == 0x8000
  // ADC_SAMPLE_TIME_31 == 0x1F00
  AD1CON3 = ADC_CONV_CLK_INTERNAL_RC | ADC_SAMPLE_TIME_31;
  // set our sample channel
  // ADC_CH0_NEG_SAMPLEA_VREFN = 0x0000
  // then or with input channel
  AD1CHS0 = ADC_CH0_NEG_SAMPLEA_VREFN | u8_channel;
  TURN_ADC_ON(); // turn on the ADC
}

// closes the ADC and signals the ADC is available
bool ESOS_SENSOR_CLOSE() {
  AD1CON1bits.ADON = 0; // turn ADC off
  ESOS_TASK_SIGNAL_AVAILABLE_ADC(); // let other tasks know ADC is avaliable
  // __esos_ClearSystemFlag(__ESOS_SYS_ADC_IS_BUSY) sets bit to 1 ?????????
  if(__ESOS_SYS_ADC_IS_BUSY) {
    return true;
  }
  else {
    return false;
  }
}


// ESOS child task for our quick read function
ESOS_CHILD_TASK(__esos_pic24_readADC, uint16_t *u16_data) {
  ESOS_TASK_BEGIN();
    SET_SAMP_BIT_ADC1(); //start sampling
    NOP(); //takes one clock to clear previous DONE flag, delay before checking.
    ESOS_TASK_WAIT_UNTIL(IS_CONVERSION_COMPLETE_ADC1()); // wait for done bit
    *u16_data = ADC1BUF0; // read the buffer
  ESOS_TASK_END();
}

// ESOS child task for our read, process, and format function
ESOS_CHILD_TASK(__esos_pic24_readSensor, uint16_t *u16_data,
                uint8_t u8_process_mask, uint8_t u8_format_mask) {
  ESOS_TASK_BEGIN();
  static uint16_t u16_sample; // to hold the current sample from the buffer this will be an un formatted result
  static uint16_t u16_prevSample; // holds the previous sample from the buffer
  static uint16_t u16_cSample; // a counter that is incremented each sample for the avg
  static uint32_t u32_avg; // the running average
  static bool b_sampleing; // set if > 1 sample is being done for prevSample purposes
  static uint16_t u16_nSample; // grabs the lower byte of process mask for sample times
  static uint16_t u16_medSamples[64]; // array to hold the number of samples for median
  static uint16_t u16_formatRef; // to set the format referance for Vref or Percentage
  static uint16_t u16_format; // to set the 12 bit or 10 bit format
  // ****************************** P R O C E S S I N G *****************************************************//
  // Processing will take the current sample from the ADC and stor it in u16_sample. This will be the raw bits
  // from the adc. We can then return the raw bits if the user passes ESOS_SENSOR_FORMAT_BITS or formatted results
  // if the user passes ESOS_SENSOR_FORMAT_VOLTAGE or ESOS_SENSOR_FORMAT_PERCENT
  //_________________________________________________________________________________________________________//
  // Extract the instructions based on u8_process_mask
  u16_nSample = (u8_process_mask &
                 0x0F); // set number of samples to lower bits of process mask
  st_adcValues.b_min = (bool) (u8_process_mask & 0x20); // 0x20 ESOS_SENSOR_MIN
  st_adcValues.b_max = (bool) (u8_process_mask & 0x40); // 0x40 ESOS_SENSOR_MAX
  st_adcValues.b_med = (bool) (u8_process_mask & 0x80); // 0x80 ESOS_SENSOR_MEDIAN
  //_________________________________________________________________________________________________________//
  st_adcValues.u16_numSample = 0x01; // set the number of samples to 1 initially
  // If we need to take more then on sample we need to determain if we should take an average, then
  // set the number of samples based on nSample. The loop does 2^nSample to set u16_numSample
  if(u16_nSample) {
    // if numSamples != 0 check to see if min, max, or med is set. If not set avg.
    st_adcValues.b_avg = !(st_adcValues.b_min | st_adcValues.b_max |
                           st_adcValues.b_med);
    while(u16_nSample) {
      st_adcValues.u16_numSample =  st_adcValues.u16_numSample << 1;
      u16_nSample--;
    }
  }
  // logic to keep track of sample information only used if taking more than one sample
  b_sampleing = FALSE;
  u16_cSample = 0;
  // Sampling starts
  while(st_adcValues.u16_numSample) {
    u16_prevSample = u16_sample; // for determaining the min and max. not used in avg or med
    SET_SAMP_BIT_ADC1(); //start sampling AD1CON1bits.SAMP=1
    NOP(); //takes one clock to clear previous DONE flag, delay before checking.
    ESOS_TASK_WAIT_UNTIL(IS_CONVERSION_COMPLETE_ADC1()); // wait until the conversion is complete before we set the sample
    u16_sample = ADC1BUF0; // read the ADC buffer
    // use minimum of x sample
    if(st_adcValues.b_min) {
      if(u16_prevSample < u16_sample && b_sampleing) { // if prevSample is less then current sample
        u16_sample = u16_prevSample; // should only happen after first sample
      }
    }
    // use max of x sample
    else if(st_adcValues.b_max) {
      if(u16_prevSample > u16_sample && b_sampleing) { // if prevSample is greater then current sample
        u16_sample = u16_prevSample; // should only happen after first sample
      }
    }
    // use average of x sample
    // This algorithum might uses a running average for sake of memory usage
    else if(st_adcValues.b_avg) { // if average bit is set
      u32_avg = (uint32_t) u16_sample + (u32_avg * (u16_cSample)); // current average = current sample + last average * n - 1
      u32_avg = u32_avg / ++u16_cSample; // current average is updated by taking average / n
      u16_sample = (uint16_t) u32_avg; // finally set the current sample back to the average in case it is the last sample
    }
    // use median of x sample
    // maybe we can find another way to do this without saving all samples
    else if(st_adcValues.b_med) {
      u16_medSamples[u16_cSample] = u16_sample;
      u16_cSample++;
    }
    st_adcValues.u16_numSample--; // decrement the sample number so we don't get stuck in a infinite loop
    b_sampleing = TRUE; // let the logic for min and max know we are sampleing
  }
  // see if we need to do calculations for the med
  if(st_adcValues.b_med) {
    // if number of samples = 2 :: take 2/2 = 1; 2/2 - 1 = 0  [x,x]
    // if number of samples = 4 :: take 4/2 = 2; 4/2 - 1 = 1  [z,x,x,z]
    // if number of samples = 8 :: take 8/2 = 4; 8/2 - 1 = 3  [z,z,z,x,x,z,z,z]
    // etc, etc, :: add those bits then shift to divide result by 2

    qsort(u16_medSamples, u16_cSample, sizeof(int), cmpfunc);
    u16_sample = (u16_medSamples[u16_cSample / 2] + u16_medSamples[(u16_cSample / 2) - 1]) >> 1;
  }
  // ****************************E N D - P R O C E S S I N G ***********************************************//

  
  //**************************** F O R M A T I N G *********************************************************//
  // check to see if we need to format, if not just skip
  if(u8_format_mask) {
    // set the format bits to 10 or 12
    if(st_adcValues.b_use12bits) {
      u16_format = FORMAT_12_BITS; // shift >> by 12 bits
    }
    else {
      u16_format = FORMAT_10_BITS; // shift >> by 12 bits
    }
    // to set the format for Vref or Percentage
    if(u8_format_mask == ESOS_SENSOR_FORMAT_PERCENT) {
      u16_formatRef = PERCENTAGE; // returns a INT from 0 - 100
    }
    else {
      u16_formatRef = get_vref(); // returns a INT number from 0 to Vref * 100
    }
    /* formula to format
          sample                result
          --------    =   ------------------
          max bits        vref or percentage

      ex: for 12 bits 3.3Vref
          max bits is 0x0FFF set vref to 330 or 0x14A               :: 0x0FFF = 4096
          if sample reads 0x0800 Vref should read 3.3 / 2 = 1.65    :: 0x0800 = 2048
          sample * Vref >> max bits = 0x00a5 = 165
    */
    u16_sample = ((uint32_t)u16_formatRef * (uint32_t)u16_sample) >> u16_format;
  }
  //**************************** E N D - F O R M A T I N G *************************************************//
  *u16_data = u16_sample; // send the results back to the request
  ESOS_TASK_END();
}