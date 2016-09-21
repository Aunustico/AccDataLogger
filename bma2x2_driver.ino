
#include <RingBuf.h>


#define BMA2X2_ADDRESS 0x18 // 0x18 or 0x19...

// define readReg and writeReg bits for SPI
#define READ_REG 0x80
#define WRITE_REG 0x7F

// register definitions
#define BGW_CHIPID 0x00
#define ACCD_X_LSB 0x02
#define ACCD_X_MSB 0x03
#define ACCD_Y_LSB 0x04
#define ACCD_Y_MSB 0x05
#define ACCD_Z_LSB 0x06
#define ACCD_Z_MSB 0x07
#define ACCD_TEMP 0x08
#define INT_STATUS_0 0x09
#define INT_STATUS_1 0x0A
#define INT_STATUS_2 0x0B
#define INT_STATUS_3 0x0C
#define FIFO_STATUS 0x0E
#define PMU_RANGE 0x0F
#define PMU_BW 0x10
#define PMU_LPW 0x11
#define PMU_LOW_POWER 0x12
#define ACCD_HBW 0x13
#define BGW_SOFTRESET 0x14
#define INT_EN_0 0x16
#define INT_EN_1 0x17
#define INT_EN_2 0x18
#define INT_MAP_0 0x19
#define INT_MAP_1 0x1A
#define INT_MAP_2 0x1B
#define INT_SRC 0x1E
#define INT_OUT_CTRL 0x20
#define INT_RST_LATCH 0x21
#define INT_0 0x22
#define INT_1 0x23
#define INT_2 0x24
#define INT_3 0x25
#define INT_4 0x26
#define INT_5 0x27
#define INT_6 0x28
#define INT_7 0x29
#define INT_8 0x2A
#define INT_9 0x2B
#define INT_A 0x2C
#define INT_B 0x2D
#define INT_C 0x2E
#define INT_D 0x2F
#define FIFO_CONFIG_0 0x30
#define PMU_SELF_TEST 0x32
#define TRIM_NVM_CTRL 0x33
#define BGW_SPI3_WDT 0x34
#define OFC_CTRL 0x36
#define OFC_SETTING 0x37
#define OFC_OFFSET_X 0x38
#define OFC_OFFSET_Y 0x39
#define OFC_OFFSET_Z 0x3A
#define TRIM_GP0 0x3B
#define TRIM_GP1 0x3C
#define FIFO_CONFIG_1 0x3E
#define FIFO_DATA 0x3F

// other definitions
#define SHADOW_ENABLE 0xBF
#define SOFT_RESET_BYTE 0xB6


#define TMP_OFFSET 23.0

// define interrupts
#define FLAT_EN 1
#define ORIENT_EN 2
#define S_TAP_EN 3
#define D_TAP_EN 4
#define SLOPE_EN_Z 5
#define SLOPE_EN_Y 6
#define SLOPE_EN_X 7
#define INT_FWM_EN 8
#define INT_FFULL_EN 9
#define DATA_EN 10
#define LOW_EN 11
#define HIGH_EN_Z 12
#define HIGH_EN_Y 13
#define HIGH_EN_X 14
#define SLO_NO_MOT_SEL 15
#define SLO_NO_MOT_EN_Z 16
#define SLO_NO_MOT_EN_Y 17
#define SLO_NO_MOT_EN_X 18

// define interrupt pins
#define INT1_FLAT 1
#define INT1_ORIENT 2
#define INT1_S_TAP 3
#define INT1_D_TAP 4
#define INT1_SLO_NO_MOT 5
#define INT1_SLOPE 6
#define INT1_HIGH 7
#define INT1_LOW 8
#define INT1_FFULL 9
#define INT1_FWM 10
#define INT1_DATA 11

#define INT2_FLAT 1
#define INT2_ORIENT 2
#define INT2_S_TAP 3
#define INT2_D_TAP 4
#define INT2_SLO_NO_MOT 5
#define INT2_SLOPE 6
#define INT2_HIGH 7
#define INT2_LOW 8
#define INT2_FFULL 9
#define INT2_FWM 10
#define INT2_DATA 11

// define ranges
#define RANGE_2G 0x03
#define RANGE_4G 0x05
#define RANGE_8G 0x08
#define RANGE_16G 0x0C

// define badwidths
#define BW_7_81_HZ 0x08
#define BW_15_63_HZ 0x09
#define BW_31_25_HZ 0x0A
#define BW_62_5_HZ 0x0B
#define BW_125_HZ 0x0C
#define BW_250_HZ 0x0D
#define BW_500_HZ 0x0E
#define BW_1000_HZ 0x0F

byte accRange = 2;
bool useSPI = false;

void Bma2x2UseI2C(void)
{
  useSPI = false;
}

void Bma2x2UseSPI(void)
{
  useSPI = true;
}



//struct sensorData
//{
//  //unsigned long counter;
//  byte xData;
//  byte yData;
//  byte zData;
//};


/*
   the configurations variables, use functions for changing the values
*/
/*
   enable (1) or disable (0) the interrupts
*/
byte flatInterrupt = 0x00;
byte orientInterrupt = 0x00;
byte singleTapInterrupt = 0x00;
byte dualtTapInterrutp = 0x00;
byte slopeInterruptZ = 0x00;
byte slopeInterruptY = 0x00;
byte slopeInterruptX = 0x00;
byte fifoWatermarkInterrupt = 0x00;
byte fifoFullInterrupt = 0x00;
byte dataReadyInterrupt = 0x00;
byte lowGInterrupt = 0x00;
byte highGInterruptZ = 0x00;
byte highGInterruptY = 0x00;
byte highGInterruptX = 0x00;
byte slowNoMotienInterruptZ = 0x00;
byte slowNoMotienInterruptY = 0x00;
byte slowNoMotienInterruptX = 0x00;
byte slowNoMotienSelect = 0x00; // slow motion (0), no motion (1)



/*
   Map the interrupt to interrupt pins 1 and 2
   0: unmap all
   1: map to 1
   2: map to 2
   3: map to 1 and 2
*/
byte flatIntMap = 0x00;
byte orientIntMap = 0x00;
byte singleTapIntMap = 0x00;
byte dualTapIntMap = 0x00;
byte slowNoMotionIntMap = 0x00;
byte slopeIntMap = 0x00;
byte highGIntMap = 0x00;
byte lowGIntMap = 0x00;
byte dataReadyIntMap = 0x00;
byte fifoWatermarkIntMap = 0x00;
byte fifoFullIntMap = 0x00;


/*
   data source definiitons
   select 0 for filtered interrupt and
   select 1 for unfiltered datainterrupt
*/
byte intSrcData = 0x00;
byte intSrcTap = 0x00;
byte intSrcSlowNoMotion = 0x00;
byte intSrcSlope = 0x00;
byte intSrcHighG = 0x00;
byte intSrcLowG = 0x00;

/*
   behavioural configuration
*/
byte int2Od = 0x00;
byte int2Lvl = 0x01;
byte int1Od = 0x00;
byte int1Lvl = 0x01;

byte pmuRange = 0x03; // byte for writing register
byte interruptLatch = 0x00;
byte pmuBandwidth = 0x0F; // byte for writing the register
byte mainPowerMode = 0x00;
byte sleepDuration = 0x00;
byte lowPowerMode = 0x00;
byte sleepTimerMode = 0x00;

/*
   data high bw from acceleration data register
   1 -> unfiltered data
   0 -> filtered data (default)
*/
byte dataHighBandwidth = 0x00;


/*
   low G interrupt trigger delay
   (low_dur + 1) * 2 ms
   (9 + 1) * 2 ms = 20 ms (default)
*/
byte lowGInterruptDelay = 0x09;
byte lowGInterruptThreshold = 0x30; // default 375 mg
byte lowGIntHighHysteresis = 0x02; // 0 -> 3
byte lowGIntMode = 0x00; // 0 or 1
byte lowGIntLowHysteresis = 0x01; // 0 -> 3
byte highGInterruptDelay = 0x0F;
byte highGInterruptThreshold = 0xC0; // default
byte slowNoMotionDuration = 0x00; // default
byte slopeDuration = 0x00; // default
byte slopeThreshold = 0x14;
byte slowNoMotionInterruptThreshold = 0x14;
byte tapQuietDuration = 0x00;
byte tapShockDuration = 0x00;
byte dualTapDuration = 0x04;
byte tapSamples = 0x00;
byte tapThreshold = 0x0A;
byte orientHysteresis = 0x01;
byte orientBlocking = 0x02;
byte orientMode = 0x00;
byte orientUdEn = 0x01;
byte orientTheta = 0x08;
byte flatTheta = 0x08;
byte flatHoldTime = 0x01;
byte flatHysteresis = 0x01;
byte fifoWatermarkLevel = 0x00;
byte fifoMode = 0x00;
byte fifoDataSelect = 0x00;
byte anyMotionInterruptThreshold = 0x14;
float accBW = 1000.0;



byte offsetTargetZ = 0x00; // 2-bit
byte offsetTargetY = 0x00;
byte offsetTargetX = 0x00;
byte offsetTargetCutOff = 0x00; // 1-bit : 1-> slower

byte offsetReset = 0x00;
byte offsetTrigger = 0x00;
byte offsetCalRdy = 0x00;
byte offsetCompensationEnableZ = 0x00;
byte offsetCompensationEnableY = 0x00;
byte offsetCompensationEnableX = 0x00;








/*
   interrupt latch
   0x00 = 0000b: non-latched
   0x01 = 0001b: temporary 250 ms
   0x02 = 0010b: temporary 500 ms
   0x03 = 0011b: temporary 1s
   0x04 = 0100b: temporary 2s
   0x05 = 0101b: temporary 4s
   0x06 = 0110b: temporary 8s
   0x07 = 0111b: latched
   0x08 = 1000b: non-latcehd
   0x09 = 1001b: temporary 250 µs
   0x0A = 1010b: temporary 500 µs
   0x0B = 1011b temporary 1 ms
   0x0C = 1100b: temporary 12.5 ms
   0x0D = 1101b: temporary 25 ms
   0x0E = 1110b: temporary 50 ms
   0x0F = 1111b: latched
*/
void Bma2x2SetInterruptLatch(byte value)
{
  value &= 0x0F;
  interruptLatch = value;
}

void Bma2x2ClearInterruptLatch(void)
{
  byte value = 0x00;
  value &= (1 << 7);
  Bma2x2WriteRegister(INT_RST_LATCH, value);
}

/*
   PMU range
   0x03 = 0011b: 2g
   0x05 = 0101b: 4g
   0x08 = 1000b: 8g
   0x0C = 1100b: 16g
   other settings are not allowed
*/


void Bma2x2SetPmuRange(byte value)
{
  value &= 0x0F;
  switch (value)
  {
    case 0x03:
      pmuRange = 0x03;
      break;
    case 0x05:
      pmuRange = 0x05;
      break;
    case 0x08:
      pmuRange = 0x08;
      break;
    case 0x0C:
      pmuRange = 0x0C;
      break;
  }
}

/*
   PMU bandwidth
   00xxxb: 7.81 Hz
   01000b: 7.81 Hz
   01001b: 15.63 Hz
   01010b: 31.25 Hz
   01011b: 62.5 Hz
   01100b: 125 Hz
   01101b: 250 Hz
   01110b: 500 Hz
   01111b: 1000 Hz (default)
   1xxxxb: 1000 Hz
*/
void Bma2x2SetPmuBandwidth(byte value)
{
  value &= 0x1F;
  pmuBandwidth = value;
}

/*
   main power mode
   {0; 0; 0}: Normal mode (default)
   {0; 0; 1}: Deep suspend
   {0; 1; 0}: low power
   {1; 0; 0}: suspend
*/
void Bma2x2SetMainPowerMode(byte value)
{
  value &= 0x07;
  mainPowerMode = value;
}

/*
   sleep duration
   0x00 -> 0x05: 0.5 ms (default)
   0x06 -> 1 ms
   0x07 -> 2 ms
   0x08 -> 4 ms
   0x09 -> 6 ms
   0x0A -> 10 ms
   0x0B -> 25 ms
   0x0C -> 50 ms
   0x0D -> 100 ms
   0x0E -> 500 ms
   0x0F -> 1 s
*/
void Bma2x2SetSleepDuration(byte value)
{
  value &= 0x0F;
  sleepDuration = value;
}

byte Bma2x2BuildPowerModeRegister(void)
{
  byte value = 0x00;
  value |= (mainPowerMode << 5);
  value |= (sleepDuration << 1);
  return value;
}


void Bma2x2SetLowPowerMode1(void)
{
  lowPowerMode = 0x00;
}
void Bma2x2SetLowPowerMode2(void)
{
  lowPowerMode = 0x01;
}

byte Bma2x2BuildLowPowerRegister(void)
{
  byte value = 0x00;
  value |= (lowPowerMode << 6);
  value |= (sleepTimerMode << 5);
  return value;
}

/*
   write the powermode to the register
   0: normal mode
   1: deep suspend mode
   2: suspend mode
   3: standby mode
   4: low power mode 1
   5: low power mode 2
   return non zero if value is not valid
*/
void Bma2x2SetPowerMode(byte powerMode)
{
  if (powerMode <= 5)
  { switch (powerMode)
    {
      case 0: // normal mode
        Bma2x2SetMainPowerMode(0x00);
        Bma2x2SetLowPowerMode1();
        break;
      case 1: // deep suspend mode
        Bma2x2SetMainPowerMode(0x01);
        Bma2x2SetLowPowerMode1();
        break;
      case 2: // suspend mode
        Bma2x2SetMainPowerMode(0x04);
        Bma2x2SetLowPowerMode1();
        break;
      case 3: // standby mode
        Bma2x2SetMainPowerMode(0x04);
        Bma2x2SetLowPowerMode2();
        break;
      case 4: // low power mode 1
        Bma2x2SetMainPowerMode(0x02);
        Bma2x2SetLowPowerMode1();
        break;
      case 5: // low power mode 2
        Bma2x2SetMainPowerMode(0x02);
        Bma2x2SetLowPowerMode2();
        break;
    }
  }
}


/*
   set value in milliseconds from 2 to 512
*/
byte Bma2x2SetLowGInterruptDelay(byte duration)
{
  if ((duration >= 2) && (duration <= 512))
  {
    lowGInterruptDelay = (duration / 2) - 1;
    return 0;
  }
  else
    return 1;
}

/*
   low g interrupt threshold
   from 0 to 1992 mg
   threshold = (low_threshold) * 7.81 mg
*/
byte Bma2x2SetLowGInterruptThreshold(float threshold)
{
  if ((threshold >= 0) && (threshold <= 1992))
  {
    byte value = 0x00;
    value |= (byte)(threshold / 7.81);
    lowGInterruptThreshold = value;
    return 0;
  }
  else
    return 1;
}

/*
   low g interrupt hysteresis
*/


void Bma2x2SetLowGIntHighHysteresis(byte value)
{
  value &= 0x03;
  lowGIntHighHysteresis = value;
}
void Bma2x2SetLowGIntMode(byte value)
{
  value &= 0x01;
  lowGIntMode = value;
}
void Bma2x2SetLowGIntLowHysteresis(byte value)
{
  value &= 0x03;
  lowGIntLowHysteresis = value;
}

byte Bma2x2BuildLowGIntHysteresisRegister(void)
{
  byte value = 0x00;
  value |= (lowGIntHighHysteresis << 6);
  value |= (lowGIntMode << 2);
  value |= (lowGIntLowHysteresis << 0);
  return value;
}






/*
   high G interrupt trigger delay
   (low_dur + 1) * 2 ms
   (15 + 1) * 2 ms = 32 ms (default)
*/
/*
   set value in milliseconds from 2 to 512
*/
byte Bma2x2SetHighGInterruptDelay(byte duration)
{
  if ((duration >= 2) && (duration <= 512))
  {
    highGInterruptDelay = (duration / 2) - 1;
    return 0;
  }
  else
    return 1;
}

/*
   high g interrupt threshold
   threshold = (high_threshold) * 7.81 mg (2g range)
   threshold = (high_threshold) * 15.63 mg (4g range)
   threshold = (high_threshold) * 31.25 mg (8g range)
   threshold = (high_threshold) * 62.5 mg (16g range)
*/

byte Bma2x2SetHighGInterruptThreshold(float threshold)
{
  switch (accRange)
  {
    case 2:
      if ((threshold >= 0) && (threshold <= 1992))
      {
        byte value = 0x00;
        value |= (byte)(threshold / 7.81);
        lowGInterruptThreshold = value;
        return 0;
      }
      else
        return 1;
      break;
    case 4:
      if ((threshold >= 0) && (threshold <= 3985))
      {
        byte value = 0x00;
        value |= (byte)(threshold / 15.63);
        lowGInterruptThreshold = value;
        return 0;
      }
      else
        return 1;
      break;
    case 8:
      if ((threshold >= 0) && (threshold <= 7969))
      {
        byte value = 0x00;
        value |= (byte)(threshold / 31.25);
        lowGInterruptThreshold = value;
        return 0;
      }
      else
        return 1;
      break;
    case 16:
      if ((threshold >= 0) && (threshold <= 15938))
      {
        byte value = 0x00;
        value |= (byte)(threshold / 62.5);
        lowGInterruptThreshold = value;
        return 0;
      }
      else
        return 1;
      break;
  }
  return 1;
}

/*
   slow and no-motion duration:
   if slow motion selected the duration defines the number of
   data points required for triggering interrupt, if no motion selected
   the duration defines time in seconds required for triggtering interrupt
   see datasheet for more details
   slope duration:
   slope interrupt triggers if slope duration +1 data points
   are above the slope interrupt threshold
*/


byte Bma2x2SetSlowNoMotionDuration(byte value)
{
  if (value <= 0x3F) {
    slowNoMotionDuration = value;
    return 0;
  }
  else
    return 1;
}

/*
   set the slope duration from 1 to 4
   return 1 if valua is not valid
*/
byte Bma2x2SetSlopeDuration(byte duration)
{
  if ((duration >= 1) && (duration <= 4))
  {
    slopeDuration = duration - 1;
    return 0;
  }
  else
    return 1;
}

/*
   any motion interrupt threshold
*/



/*
   get the byte for interrupt threshold in lower range
*/
byte Bma2x2InterruptThresholdHelper1(float threshold)
{
  byte returnValue = 0x00;
  switch (accRange)
  {
    case 2:
      if ((threshold >= 0) && (threshold <= 1992))
      {
        byte value = 0x00;
        value |= (byte)(threshold / 7.81);
        returnValue = value;
      }
      break;
    case 4:
      if ((threshold >= 0) && (threshold <= 3985))
      {
        byte value = 0x00;
        value |= (byte)(threshold / 15.63);
        returnValue = value;
      }
      break;
    case 8:
      if ((threshold >= 0) && (threshold <= 7969))
      {
        byte value = 0x00;
        value |= (byte)(threshold / 31.25);
        returnValue = value;
      }
      break;
    case 16:
      if ((threshold >= 0) && (threshold <= 15938))
      {
        byte value = 0x00;
        value |= (byte)(threshold / 62.5);
        returnValue = value;
      }
      break;
  }
  return returnValue;
}

/*
   get the byte for interrupt threshold in the higher range
*/
byte Bma2x2InterruptThresholdHelper2(float threshold)
{
  byte returnValue = 0x00;
  switch (accRange)
  {
    case 2:
      if ((threshold >= 0) && (threshold <= 1992))
      {
        byte value = 0x00;
        value |= (byte)(threshold / 15.63);
        returnValue = value;
      }
      break;
    case 4:
      if ((threshold >= 0) && (threshold <= 3985))
      {
        byte value = 0x00;
        value |= (byte)(threshold / 31.25);
        returnValue = value;
      }
      break;
    case 8:
      if ((threshold >= 0) && (threshold <= 7969))
      {
        byte value = 0x00;
        value |= (byte)(threshold / 62.5);
        returnValue = value;
      }
      break;
    case 16:
      if ((threshold >= 0) && (threshold <= 15938))
      {
        byte value = 0x00;
        value |= (byte)(threshold / 125.0);
        returnValue = value;
      }
      break;
  }
  return returnValue;
}
















/*
   other variables
*/


/*
   function for enabling and disabling interrupts
*/
// flat interrupt
void Bma2x2EnableFlatInterrupt(void)
{
  flatInterrupt = 0x01;
}
void Bma2x2DisableFlatInterrupt(void)
{
  flatInterrupt = 0x00;
}

// orient interrutp
void Bma2x2EnableOrientInterrupt(void)
{
  orientInterrupt = 0x01;
}
void Bma2x2DisableOrientInterrupt(void)
{
  orientInterrupt = 0x00;
}

// single tap interrupt
void Bma2x2EnableSingleTapInterrupt(void)
{
  singleTapInterrupt = 0x01;
}

void Bma2x2DisableSingleTapInterrupt(void)
{
  singleTapInterrupt = 0x00;
}

// dual tap interrupt
void Bma2x2EnableDualTapInterrupt(void)
{
  dualtTapInterrutp = 0x01;
}
void Bma2x2DisableDualTapInterrupt(void)
{
  dualtTapInterrutp = 0x00;
}

// slope interrupt Z
void Bma2x2EnableSlopeInterruptZ(void)
{
  slopeInterruptZ = 0x01;
}
void Bma2x2DisableSlopeInterruptZ(void)
{
  slopeInterruptZ = 0x00;
}

// slope interrupt Y
void Bma2x2EnableSlopeInterruptY(void)
{
  slopeInterruptY = 0x01;
}
void Bma2x2DisableSlopeInterruptY(void)
{
  slopeInterruptY = 0x00;
}

// slope interrupt X
void Bma2x2EnableSlopeInterruptX(void)
{
  slopeInterruptX = 0x01;
}
void Bma2x2DisableSlopeInterruptX(void)
{
  slopeInterruptX = 0x00;
}

// fifo watermark interrupt
void Bma2x2EnableFifoWaterMarkInterrupt(void)
{
  fifoWatermarkInterrupt = 0x01;
}
void Bma2x2DisableFifoWaterMarkInterrupt(void)
{
  fifoWatermarkInterrupt = 0x00;
}

//fifo full interrupt
void Bma2x2EnableFifoFullInterrupt(void)
{
  fifoFullInterrupt = 0x01;
}
void Bma2x2DisableFifoFullInterrupt(void)
{
  fifoFullInterrupt = 0x00;
}

// data ready interrupt
void Bma2x2EnableDataReadyInterrupt(void)
{
  dataReadyInterrupt = 0x01;
}
void Bma2x2DisableDataReadyInterrupt(void)
{
  dataReadyInterrupt = 0x00;
}

// low G interrupt
void Bma2x2EnableLowGInterrupt(void)
{
  lowGInterrupt = 0x01;
}
void Bma2x2DisableLowGInterrupt(void)
{
  lowGInterrupt = 0x00;
}

//byte high G interrupt Z
void Bma2x2EnableHighGInterruptZ(void)
{
  highGInterruptZ = 0x01;
}
void Bma2x2DisableHighGInterruptZ(void)
{
  highGInterruptZ = 0x00;
}

//byte high G interrupt Y
void Bma2x2EnableHighGInterruptY(void)
{
  highGInterruptY = 0x01;
}
void Bma2x2DisableHighGInterruptY(void)
{
  highGInterruptY = 0x00;
}

//byte high G interrupt X
void Bma2x2EnableHighGInterruptX(void)
{
  highGInterruptX = 0x01;
}
void Bma2x2DisableHighGInterruptX(void)
{
  highGInterruptX = 0x00;
}

// slow / no-motion interrupt Z
void Bma2x2EnableSlowNoMotionInterruptZ(void)
{
  slowNoMotienInterruptZ = 0x01;
}
void Bma2x2DisableSlowNoMotionInterruptZ(void)
{
  slowNoMotienInterruptZ = 0x00;
}

// slow / no-motion interrupt Y
void Bma2x2EnableSlowNoMotionInterruptY(void)
{
  slowNoMotienInterruptY = 0x01;
}
void Bma2x2DisableSlowNoMotionInterruptY(void)
{
  slowNoMotienInterruptY = 0x00;
}

// slow / no-motion interrupt X
void Bma2x2EnableSlowNoMotionInterruptX(void)
{
  slowNoMotienInterruptX = 0x01;
}
void Bma2x2DisableSlowNoMotionInterruptX(void)
{
  slowNoMotienInterruptX = 0x00;
}

// slow no-motion select
void Bma2x2SelectSlowMotionInterrupt(void)
{
  slowNoMotienSelect = 0x00;
}
void Bma2x2SelectNoMotionInterrupt(void)
{
  slowNoMotienSelect = 0x01;
}


// new data interrupt filtering
void Bma2x2EnableNewDataIntFiltering(void)
{
  intSrcData = 0x00;
}
void Bma2x2DisableNewDataIntFiltering(void)
{
  intSrcData = 0x01;
}

// tap interrupt filtering
void Bma2x2EnableTapInterruptFiltering(void)
{
  intSrcTap = 0x00;
}
void Bma2x2DisableTapInterruptFiltering(void)
{
  intSrcTap = 0x01;
}

// slow/no-motion interrupt data filtering
void Bma2x2EnableSlowNoMotionInterruptFiltering(void)
{
  byte intSrcSlowNoMotion = 0x00;
}
void Bma2x2DisableSlowNoMotionInterruptFiltering(void)
{
  byte intSrcSlowNoMotion = 0x01;
}

// slope interrupt data filtering
void Bma2x2EnableSlopeInterruptFiltering(void)
{
  byte intSrcSlope = 0x00;
}
void Bma2x2DisableSlopeInterruptFiltering(void)
{
  byte intSrcSlope = 0x01;
}

// high G interrupt filtering
void Bma2x2EnableGighGInterruptFiltering(void)
{
  byte intSrcHighG = 0x00;
}
void Bma2x2DisableGighGInterruptFiltering(void)
{
  byte intSrcHighG = 0x01;
}

// low G interrupt filtering
void Bma2x2EnableLowGInterruptFiltering(void)
{
  intSrcLowG = 0x00;
}
void Bma2x2DisableLowGInterruptFiltering(void)
{
  intSrcLowG = 0x01;
}

// set interrupt pin 2 behaviour
void Bma2x2SetInt2OpenDrain(void)
{
  int2Od = 0x01;
}
void Bma2x2SetInt2PushPull(void)
{
  int2Od = 0x00;
}

// set interrupt pin 2 level
void Bma2x2SetInt2ActiveHigh(void)
{
  int2Lvl = 0x01;
}
void Bma2x2SetInt2ActiveLow(void)
{
  int2Lvl = 0x00;
}

// set interrupt pin 1 behaviour
void Bma2x2SetInt1OpenDrain(void)
{
  int1Od = 0x01;
}
void Bma2x2SetInt1PushPull(void)
{
  int1Od = 0x00;
}

// set interrupt pin 1 level
void Bma2x2SetInt1ActiveHigh(void)
{
  int1Lvl = 0x01;
}
void Bma2x2SetInt1ActiveLow(void)
{
  int1Lvl = 0x00;
}


/*
   functions for mapping the interrupts, return 1 if value is not valid
   else return 0
   0: map none
   1: map to pin 1
   2: map to pin 2
   3: map to pins 1 and 2
   the function changes the variable only if the value is valid,
   otherwise it remains as it was
*/

// map flat interrupt
byte Bma2x2MapFlatInt(byte value)
{
  if (value > 3)
    return 1;
  else {
    value &= 0x03; // mask the two LSB bits
    flatIntMap = value;
    return 0;
  }
}

// map orient interrupt
byte Bma2x2MapOrientInt(byte value)
{
  if (value > 3)
    return 1;
  else {
    value &= 0x03; // mask the two LSB bits
    orientIntMap = value;
    return 0;
  }
}

// map single tap interrupt
byte Bma2x2MapSingleTapInt(byte value)
{
  if (value > 3)
    return 1;
  else {
    value &= 0x03; // mask the two LSB bits
    singleTapIntMap = value;
    return 0;
  }
}

// map dual tap interrupt
byte Bma2x2MapDualTapInt(byte value)
{
  if (value > 3)
    return 1;
  else {
    value &= 0x03; // mask the two LSB bits
    dualTapIntMap = value;
    return 0;
  }
}

// map slow no-motion interrupt
byte Bma2x2MapSlowNoMotionInt(byte value)
{
  if (value > 3)
    return 1;
  else {
    value &= 0x03; // mask the two LSB bits
    slowNoMotionIntMap = value;
    return 0;
  }
}

// map slope interrupt
byte Bma2x2MapSlopeInt(byte value)
{
  if (value > 3)
    return 1;
  else {
    value &= 0x03; // mask the two LSB bits
    slopeIntMap = value;
    return 0;
  }
}

// map high G interrupt
byte Bma2x2MapHighGInt(byte value)
{
  if (value > 3)
    return 1;
  else {
    value &= 0x03; // mask the two LSB bits
    highGIntMap = value;
    return 0;
  }
}

// map low G interrupt
byte Bma2x2MapLowGInt(byte value)
{
  if (value > 3)
    return 1;
  else {
    value &= 0x03; // mask the two LSB bits
    lowGIntMap = value;
    return 0;
  }
}

// map data ready interrupt
byte Bma2x2MapDataReadyInt(byte value)
{
  if (value > 3)
    return 1;
  else {
    value &= 0x03; // mask the two LSB bits
    dataReadyIntMap = value;
    return 0;
  }
}

// map fifo watermark ready interrupt
byte Bma2x2MapFifoWatermarkInt(byte value)
{
  if (value > 3)
    return 1;
  else {
    value &= 0x03; // mask the two LSB bits
    fifoWatermarkIntMap = value;
    return 0;
  }
}

// map fifo full ready interrupt
byte Bma2x2MapFifoFullInt(byte value)
{
  if (value > 3)
    return 1;
  else {
    value &= 0x03; // mask the two LSB bits
    fifoFullIntMap = value;
    return 0;
  }
}

/*
   creates a byte for writing the register from the configurations
*/
// INT_EN_0 register
byte Bma2x2BuildIntEn0Register(void)
{
  byte value = 0x00;
  value |= (flatInterrupt << 7);
  value |= (orientInterrupt << 6);
  value |= (singleTapInterrupt << 5);
  value |= (dualtTapInterrutp << 4);
  value |= (slopeInterruptZ << 2);
  value |= (slopeInterruptY << 1);
  value |= (slopeInterruptX << 0);
  return value;
}

// INT_EN_1 register
byte Bma2x2BuildIntEn1Register(void)
{
  byte value = 0x00;
  value |= (fifoWatermarkInterrupt << 6);
  value |= (fifoFullInterrupt << 5);
  value |= (dataReadyInterrupt << 4);
  value |= (lowGInterrupt << 3);
  value |= (highGInterruptZ << 2);
  value |= (highGInterruptY << 1);
  value |= (highGInterruptX << 0);
  return value;
}

// INT_EN_2 register
byte Bma2x2BuildIntEn2Register(void)
{
  byte value = 0x00;
  value |= (slowNoMotienSelect << 3);
  value |= (slowNoMotienInterruptZ << 2);
  value |= (slowNoMotienInterruptY << 1);
  value |= (slowNoMotienInterruptX << 0);
  return value;
}


/*
   Create the Map registers
*/

byte Bma2x2BuildIntMap0Register(void)
{
  byte value = 0x00;
  value |= ((flatIntMap & 0x01) << 7);
  value |= ((orientIntMap & 0x01) << 6);
  value |= ((singleTapIntMap & 0x01) << 5);
  value |= ((dualTapIntMap & 0x01) << 4);
  value |= ((slowNoMotionIntMap & 0x01) << 3);
  value |= ((slopeIntMap & 0x01) << 2);
  value |= ((highGIntMap & 0x01) << 1);
  value |= ((lowGIntMap & 0x01) << 0);
  return value;
}

byte Bma2x2BuildIntMap1Register(void)
{
  byte value = 0x00;
  value |= ((dataReadyIntMap & 0x02) << 6);
  value |= ((fifoWatermarkIntMap & 0x02) << 5);
  value |= ((fifoFullIntMap & 0x02) << 4);
  value |= ((fifoFullIntMap & 0x01) << 2);
  value |= ((fifoWatermarkIntMap & 0x01) << 1);
  value |= ((dataReadyIntMap & 0x01) << 0);
  return value;
}

byte Bma2x2BuildIntMap2Register(void)
{
  byte value = 0x00;
  value |= ((flatIntMap & 0x02) << 6);
  value |= ((orientIntMap & 0x02) << 5);
  value |= ((singleTapIntMap & 0x02) << 4);
  value |= ((dualTapIntMap & 0x02) << 3);
  value |= ((slowNoMotionIntMap & 0x02) << 2);
  value |= ((slopeIntMap & 0x02) << 1);
  value |= ((highGIntMap & 0x02) << 0);
  value |= ((lowGIntMap & 0x02) >> 1);
  return value;
}

byte Bma2x2BuildIntSrcRegister(void)
{
  byte value = 0x00;
  value |= (intSrcData << 5);
  value |= (intSrcTap << 4);
  value |= (intSrcSlowNoMotion << 3);
  value |= (intSrcSlope << 2);
  value |= (intSrcHighG << 1);
  value |= (intSrcLowG << 0);
  return value;
}

byte Bma2x2BuildIntOutCtrlRegister(void)
{
  byte value = 0x00;
  value |= (int2Od << 3);
  value |= (int2Lvl << 2);
  value |= (int1Od << 1);
  value |= (int1Lvl << 0);
  return value;
}


byte Bma2x2BuildInt2Register(void)
{
  byte value = 0x00;
  value |= (lowGIntHighHysteresis << 6);
  value |= (lowGIntMode << 2);
  value |= lowGIntLowHysteresis;
  return value;
}

byte Bma2x2BuildInt5Register(void)
{
  byte value = 0x00;
  value |= (slowNoMotionDuration << 2);
  value |= slopeDuration;
  return value;
}

byte Bma2x2BuildInt8Register(void)
{
  byte value = 0x00;
  value |= (tapQuietDuration << 7);
  value |= (tapShockDuration << 6);
  value |= dualTapDuration;
  return value;
}

byte Bma2x2BuildInt9Register(void)
{
  byte value = 0x00;
  value |= (tapSamples << 6);
  value |= tapThreshold;
  return value;
}

byte Bma2x2BuildIntARegister(void)
{
  byte value = 0x00;
  value |= (orientHysteresis << 4);
  value |= (orientBlocking << 2);
  value |= orientMode;
  return value;
}

byte Bma2x2BuildIntBRegister(void)
{
  byte value = 0x00;
  value |= (orientUdEn << 6);
  value |= orientTheta;
  return value;
}

byte Bma2x2BuildIntDRegister(void)
{
  byte value = 0x00;
  value |= (flatHoldTime << 4);
  value |= flatHysteresis;
  return value;
}



/*
	write soft reset byte to soft reset register
	resets most of the settings
*/
void Bma2x2SoftReset(void)
{
  Bma2x2WriteRegister(BGW_SOFTRESET, SOFT_RESET_BYTE);
}


byte Bma2x2BuildAccDataAcquisitionRegister(void)
{
  byte value = 0x00;
  value |= (dataHighBandwidth << 7);
  return value;
}
/*
	functin for enabling shadowing,
	required for new data flag
*/
void Bma2x2EnableShadowing(void)
{
  byte value = 0x00; // Bma2x2ReadRegister(ACCD_HBW); // read the register content
  value |= (dataHighBandwidth << 7);
  //value &= SHADOW_ENABLE; // clear the shadowing disable bit
  Bma2x2WriteRegister( ACCD_HBW,  value); // write the content to the register
}

void Bma2x2EnableAccDataFiltering(void)
{
  dataHighBandwidth = 0x00;
}

void Bma2x2DisableAccDataFiltering(void)
{
  dataHighBandwidth = 0x01;
}

void Bma2x2SetFifoWatermarkLevel(byte level)
{
  if ((level >= 0) && (level <= 32))
  {
    fifoWatermarkLevel = level;
  }
}

void Bma2x2SetFifoModeBypass(void)
{
  fifoMode = 0x00;
}

void Bma2x2SetFifoModeFifo(void)
{
  fifoMode = 0x01;
}

void Bma2x2SetFifoModeStream(void)
{
  fifoMode = 0x10;
}

void Bma2x2SelectFifoDataXYZ(void)
{
  fifoDataSelect = 0x00;
}

void Bma2x2SelectFifoDataX(void)
{
  fifoDataSelect = 0x01;
}

void Bma2x2SelectFifoDataY(void)
{
  fifoDataSelect = 0x10;
}

void Bma2x2SelectFifoDataZ(void)
{
  fifoDataSelect = 0x11;
}

byte Bma2x2BuildFifoConfig1Register(void)
{
  byte value = 0x00;
  value |= (fifoMode << 6);
  value |= fifoDataSelect;
  return value;
}


/*
	read raw acceleration data from the register and convert it into floating point value
  the rutned value is the new data status:
  1: indicates the data has been updated since last reading
  0: indicates the data is not updated
*/
byte Bma2x2GetDataZ(byte *rawData, float *sensorData)
{
  Bma2x2EnableShadowing();
  byte isNewData = 0x00;
  Bma2x2ReadRegister(ACCD_Z_LSB, &isNewData);
  byte data = 0x00;
  Bma2x2ReadRegister(ACCD_Z_MSB, &data);
  *rawData = data;
  Bma2x2AccDataConversion(data, sensorData);
  return isNewData;
}


byte Bma2x2GetDataY(byte *rawData, float *sensorData)
{
  Bma2x2EnableShadowing();
  byte isNewData = 0x00;
  Bma2x2ReadRegister(ACCD_Y_LSB, &isNewData);
  byte data = 0x00;
  Bma2x2ReadRegister(ACCD_Y_MSB, &data);
  *rawData = data;
  Bma2x2AccDataConversion(data, sensorData);
  return isNewData;
}

byte Bma2x2GetDataX(byte *rawData, float *sensorData)
{
  Bma2x2EnableShadowing();
  byte isNewData = 0x00;
  Bma2x2ReadRegister(ACCD_X_LSB, &isNewData);
  byte data = 0x00;
  Bma2x2ReadRegister(ACCD_X_MSB, &data);
  *rawData = data;
  Bma2x2AccDataConversion(data, sensorData);
  return isNewData;
}


/*
	function for reading the raw temperature data from the sensorData
  function returns nothing
*/
void Bma2x2GetTemperature(float *sensorData)
{
  byte rawData = 0x00;
  Bma2x2ReadRegister(ACCD_TEMP, &rawData); // read the raw data
  Bma2x2TemperatureDataConversion(rawData, sensorData); // convert raw data into floating point value
}

void Bma2x2GetRawTemp(byte *data)
{
  Bma2x2ReadRegister(ACCD_TEMP, data);
}

/*
	function to convert temperature data into floating point
*/
void Bma2x2TemperatureDataConversion(byte rawData, float *sensorData)
{
  signed char data = 0x00;
  data |= rawData;
  float tmpData = (float)data;
  *sensorData = TMP_OFFSET + 0.5 * tmpData;
}

/*
	function to writing floating point acceleration data to given pointer
*/
void Bma2x2AccDataConversion(byte rawData, float *sensorData)
{
  signed char data = 0x00;
  data |= rawData;
  float tmpData = (float)data;
  switch (accRange) {
    case 2:
      tmpData = tmpData * 15.63;
      break;
    case 4:
      tmpData = tmpData * 31.25;
      break;
    case 8:
      tmpData = tmpData * 62.50;
      break;
    case 16:
      tmpData = tmpData * 125.0;
      break;
  }
  *sensorData = tmpData;
}


/*
   offset compensation control
*/
//byte offsetReset = 0x00;
//byte offsetTrigger = 0x00;
//byte offsetCalRdy = 0x00; // read only
//byte offsetCompensationEnableZ = 0x00;
//byte offsetCompensationEnableY = 0x00;
//byte offsetCompensationEnableX = 0x00;

void Bma2x2ResetOffseetCompensation(void)
{
  offsetReset = 0x01;
}

void Bma2x2EnableOffsetCompensationZ(void)
{
  offsetCompensationEnableZ = 0x01;
}

void Bma2x2DisableOffsetCompensationZ(void)
{
  offsetCompensationEnableZ = 0x00;
}

void Bma2x2EnableOffsetCompensationY(void)
{
  offsetCompensationEnableY = 0x01;
}

void Bma2x2DisableOffsetCompensationY(void)
{
  offsetCompensationEnableY = 0x00;
}

void Bma2x2EnableOffsetCompensationX(void)
{
  offsetCompensationEnableX = 0x01;
}

void Bma2x2DisableOffsetCompensationX(void)
{
  offsetCompensationEnableX = 0x00;
}

byte Bma2x2BuildOffsetCompensationControl(void)
{
  byte value = 0x00;
  value |= (offsetReset << 7);
  value |= (offsetTrigger << 5);
  value |= (offsetCompensationEnableZ << 2);
  value |= (offsetCompensationEnableY << 1);
  value |= (offsetCompensationEnableX << 0);
  //Serial.print("Offset compensation control register: ");
  //Serial.println(value, BIN);
  return value;
}

/*
   offset compensation settings
*/

// byte offsetTargetZ = 0x00; // 2-bit
//byte offsetTargetY = 0x00;
//byte offsetTargetX = 0x00;
//byte offsetTargetCutOff = 0x00; // 1-bit : 1-> slower

byte Bma2x2SetFilterBandwidthHigh(void)
{
  offsetTargetCutOff = 0x01; // 10xBW
}

byte Bma2x2SetFilterBandwidthLow(void)
{
  offsetTargetCutOff = 0x00; // 1xBW
}

byte Bma2x2BuildOffsetCompensationSettings(void)
{
  byte value = 0x00;
  value |= (offsetTargetZ << 5);
  value |= (offsetTargetY << 3);
  value |= (offsetTargetX << 1);
  value |= offsetTargetCutOff;
  //Serial.print("Offset compensation settings register: ");
  //Serial.println(value, BIN);
  return value;
}

void Bma2x2GetFifoStatus(byte *fifoOverrrun, byte *fifoLevel)
{
  byte value = 0x00;
  Bma2x2ReadRegister(FIFO_STATUS, &value);
  *fifoOverrrun = (value & (1 << 7)) >> 7;
  *fifoLevel = value & ~(1 << 7);
}


/*
	function for writing one byte of data to given register address
*/
void Bma2x2WriteRegister(byte registerAddress, byte value)
{
  if (useSPI)
  {
    digitalWrite(ACC_SLAVESELECT, LOW);
    SPI.transfer((registerAddress << 1) & WRITE_REG);
    SPI.transfer(value);
    digitalWrite(ACC_SLAVESELECT, HIGH);
  }
  else
  {
    Wire.beginTransmission(BMA2X2_ADDRESS);
    Wire.write(registerAddress); // pointer register
    Wire.write(value);
    Wire.endTransmission();
  }
}

/*
	function reads oen byte of data from the register
	return value is the content of the register
*/
void Bma2x2ReadRegister(byte registerAddress, byte *value)
{
  if (useSPI)
  {
    digitalWrite(ACC_SLAVESELECT, LOW);
    SPI.transfer(registerAddress | READ_REG);
    *value = SPI.transfer(0x00);
    digitalWrite(ACC_SLAVESELECT, HIGH);
  }
  else
  {
    Wire.beginTransmission(BMA2X2_ADDRESS);
    Wire.write(registerAddress); // pointer register
    Wire.endTransmission();
    Wire.requestFrom(BMA2X2_ADDRESS, 1);
    if (Wire.available())
      *value = Wire.read();
  }
}





/*
   read burst of frames into a buffer
   returns the number of missed frames that did not fit into  the buffer
   when buffer isd full
*/
byte Bma2x2ReadBurstToBuffer(byte framesToRead, RingBuf *dataBuffer)
{
  byte returnValue = 0;
  byte idx = 0;
  byte data = 0x00;
  if (useSPI)
  {
    digitalWrite(ACC_SLAVESELECT, LOW);
    SPI.transfer(FIFO_DATA | READ_REG);
    while (framesToRead > 0)
    {
      if (dataBuffer->isFull(dataBuffer))
      {
        //Serial.println("Data buffer is full!");
        for (int i = 0; i < 6; i++)
          SPI.transfer(0x00); // clear buffer
        returnValue++;
      }
      else
      {
        sensorData data;
        SPI.transfer(0x00); // discard this information
        data.xData = SPI.transfer(0x00);
        SPI.transfer(0x00); // discard this information
        data.yData = SPI.transfer(0x00);
        SPI.transfer(0x00); // discard this information
        data.zData = SPI.transfer(0x00);
        dataBuffer->add(dataBuffer, &data);
      }
      framesToRead--;
    }
    digitalWrite(ACC_SLAVESELECT, HIGH);
  }
  else
  {
    Wire.beginTransmission(BMA2X2_ADDRESS);
    Wire.write(FIFO_DATA);
    Wire.endTransmission();
    int bytesToRead = 6 * (int)(framesToRead);
    //    Serial.print("Reading ");
    //    Serial.println(bytesToRead);
    Wire.requestFrom(BMA2X2_ADDRESS, bytesToRead);
    while (6 <= Wire.available())
    {
      //Serial.println(Wire.available());
      if (dataBuffer->isFull(dataBuffer))
      {
        //Serial.println("Data buffer is full!");
        for (int i = 0; i < 6; i++)
          Wire.read(); // clear buffer
        returnValue++;
      }
      else
      {
        sensorData data;
        Wire.read();
        data.xData = Wire.read();
        Wire.read(); // discard this information
        data.yData = Wire.read();
        Wire.read(); // discard this information
        data.zData = Wire.read();
        dataBuffer->add(dataBuffer, &data);
      }
      framesToRead--;
    }
  }
  return returnValue;
}



void Bma2x2WriteFifoConfig1Register(void)
{
  Bma2x2WriteRegister(FIFO_CONFIG_1, Bma2x2BuildFifoConfig1Register());
}


void Bma2x2WriteAllRegisters(void)
{
  // write PMU range to register
  Bma2x2WriteRegister(PMU_RANGE, pmuRange);
  Bma2x2WriteRegister(PMU_BW, pmuBandwidth);

  // write power mode registers
  Bma2x2WriteRegister(PMU_LPW, Bma2x2BuildPowerModeRegister());
  Bma2x2WriteRegister(PMU_LOW_POWER, Bma2x2BuildLowPowerRegister());
  Bma2x2WriteRegister(ACCD_HBW, Bma2x2BuildAccDataAcquisitionRegister());
  delay(10);


  // Make the interrupt settings
  Bma2x2WriteRegister(INT_0, lowGInterruptDelay); // LOW G trigger delay
  Bma2x2WriteRegister(INT_1, lowGInterruptThreshold); // low g interrupt threshold
  Bma2x2WriteRegister(INT_2, Bma2x2BuildInt2Register()); // low g interrupt hysteresis
  Bma2x2WriteRegister(INT_3, highGInterruptDelay); // high g interrupt delay
  Bma2x2WriteRegister(INT_4, highGInterruptThreshold); // high g interrupt delay
  Bma2x2WriteRegister(INT_5, Bma2x2BuildInt5Register());
  Bma2x2WriteRegister(INT_6, anyMotionInterruptThreshold);
  Bma2x2WriteRegister(INT_7, slowNoMotionInterruptThreshold);
  Bma2x2WriteRegister(INT_8, Bma2x2BuildInt8Register());
  Bma2x2WriteRegister(INT_9, Bma2x2BuildInt9Register());
  Bma2x2WriteRegister(INT_A, Bma2x2BuildIntARegister());
  Bma2x2WriteRegister(INT_B, Bma2x2BuildIntBRegister());
  Bma2x2WriteRegister(INT_C, flatTheta);
  Bma2x2WriteRegister(INT_D, Bma2x2BuildIntDRegister());

  // fifo configuraton
  Bma2x2WriteRegister(FIFO_CONFIG_0, fifoWatermarkLevel); // watermark level
  Bma2x2WriteRegister(PMU_SELF_TEST, 0x00);
  Bma2x2WriteRegister(TRIM_NVM_CTRL, 0x00);
  Bma2x2WriteRegister(BGW_SPI3_WDT, 0x00);
  Bma2x2WriteRegister(OFC_CTRL, Bma2x2BuildOffsetCompensationControl());
  Bma2x2WriteRegister(OFC_SETTING, Bma2x2BuildOffsetCompensationSettings());
  Bma2x2WriteRegister(OFC_OFFSET_X, 0x00);
  Bma2x2WriteRegister(OFC_OFFSET_Y, 0x00);
  Bma2x2WriteRegister(OFC_OFFSET_Z, 0x00);

  Bma2x2WriteRegister(TRIM_GP0, 0x00);
  Bma2x2WriteRegister(TRIM_GP1, 0x00);
  Bma2x2WriteRegister(FIFO_CONFIG_1, Bma2x2BuildFifoConfig1Register());
  Bma2x2WriteRegister(FIFO_DATA, 0x00);

  delay(10);
  // enable interrupts and map the pins
  Bma2x2WriteRegister(INT_EN_0 , Bma2x2BuildIntEn0Register());
  Bma2x2WriteRegister(INT_EN_1 , Bma2x2BuildIntEn1Register());
  Bma2x2WriteRegister(INT_EN_2 , Bma2x2BuildIntEn2Register());
  Bma2x2WriteRegister(INT_MAP_0 , Bma2x2BuildIntMap0Register());
  Bma2x2WriteRegister(INT_MAP_1 , Bma2x2BuildIntMap1Register());
  Bma2x2WriteRegister(INT_MAP_2 , Bma2x2BuildIntMap2Register());
  Bma2x2WriteRegister(INT_SRC , Bma2x2BuildIntSrcRegister());
  Bma2x2WriteRegister(INT_OUT_CTRL , Bma2x2BuildIntOutCtrlRegister());
  Bma2x2WriteRegister(INT_RST_LATCH, interruptLatch);
}


/*
   Initializez the sensor
   returns zero if the intialization is OK, otherwise
   returns the number of failures
*/
byte Bma2x2Init(void) {
  Bma2x2SoftReset(); // reset configurations

  Bma2x2SetPowerMode(0);
  //Bma2x2SetInterruptLatch(0x00); // 12.5ms latching
  //Bma2x2EnableAccDataFiltering();


  //Bma2x2EnableSingleTapInterrupt();
  //Bma2x2MapSingleTapInt(2);

  //Bma2x2EnableFifoFullInterrupt();
  //Bma2x2MapFifoFullInt(2);
  Bma2x2SetFifoModeFifo();
  Bma2x2SelectFifoDataXYZ();
  //Bma2x2SetFifoWatermarkLevel(16);

  Bma2x2MapFifoWatermarkInt(2);
  Bma2x2EnableFifoWaterMarkInterrupt();

  //  Bma2x2EnableOffsetCompensationZ();
  //  Bma2x2EnableOffsetCompensationY();
  //  Bma2x2EnableOffsetCompensationX();

  Bma2x2SetPmuBandwidth(0x0B); // 0x0A = 16ms update time 0x0B for 8 ms update time
  Bma2x2WriteAllRegisters();

  // validate the registers
  return 0;
}
