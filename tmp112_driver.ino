#define TMP112_ADDRESS 0x49
#define TMP112_READ_TMP_REGISTER 0x00 // read-only
#define TMP112_CONF_REGISTER 0x01 // read / write
#define TMP112_TEMP_LOW_REGISTER 0x02 // read / write
#define TMP112_TEMP_HIGH_REGISTER 0x03 // read / write

//#define TMP112_READ_ACCESS 0x00
//#define TMP112_WRITE_ACCESS 0x00
#define TMP112_LSB_VALUE 0.0625

/*
   Variables for storing the temperature values
*/
float temperatureFloat = 0.0;
unsigned short temperatureRaw = 0x0000;


/*
   The variables for temperature limits, use functions to
   change the limit values
*/
unsigned short tempHighLimit = 0x0000;
unsigned short tempLowLimit = 0x0000;

/*
   The configuration variables, use the functions to change these
*/
byte shutDownMode = 0x00;
byte thermostatMode = 0x00;
byte polarity = 0x00; // default
byte faultQueue = 0x00; // (F0, F1)
byte converterResolution = 0x03; // (0x03 default) read-only (R0, R1)
byte oneShot = 0x00;
byte extendedMode = 0x01;
byte alert = 0x00; // read-only
byte conversionRate = 0x02; // default (4 Hz) (CR0, CR1)

/*
   Initializes the sensor with desired settings:
   First, make the configurations
   Last, write the configurations to the register
*/
void Tmp112Init(void)
{
  /*
     Set the configuration before writing the to the configuration register
  */
  Tmp112DisableShutDownMode(); // disable power saving
  Tmp112SetComparatorMode(); // use comparator mode
  Tmp112SetNormalPolarity(); // use normal polarity for alert pin
  Tmp112SetFaultQueue(4); // set the number of fault conditions to launch an alert: 1,2,4 or 6 allowed
  //Tmp112EnableOneShot(); // Use only in power saving mode
  Tmp112EnableExtendedMode(); // use normal 12-bit mode (max 128 C)
  Tmp112SetConversionRate(2); // 2: 4 Hz (default), values from 0 to 3 are allowed

  /*
     Set the temperature limits for writing them to register
  */
  Tmp112SetHighLimit(28.0);
  Tmp112SetLowLimit(27.0);

  /*
     write configurations to the coonfiguration register
  */
  Tmp112WriteConfigurationRegister();

  /*
     write the temperature limits to the temperature limit
     registers
  */
  Tmp112WriteTempHighLimitRegister();
  Tmp112WriteTempLowLimitRegister();

  /*
     read and validate the content of the configuration register
     if a configuration mistmatches the return value differs from zero
  */
//  byte returnValue = Tmp112DecodeConfigurationRegister();
//  Serial.print("Number of false register values: ");
//  Serial.println(returnValue, DEC);
}
/*
   a power saving mode diables all other functionality except serial interface
*/
void Tmp112EnableShutDownMode(void)
{
  shutDownMode = 0x01;
}

/*
   normal power mode, all functionality
*/
void Tmp112DisableShutDownMode(void)
{
  shutDownMode = 0x00;
}

/*
   Set the functionality to comparator mode,
   temp high limit causes alert and temp low
   limit clears alert
*/
void Tmp112SetComparatorMode(void)
{
  thermostatMode = 0x00;
}

/*
   Set functionality to interrupt mode,
   temp high and temp low limits causes alerts
*/
void Tmp112SetInterruptMode(void)
{
  thermostatMode = 0x01;
}

/*
   sets the polarity of the interrupt pin, in normal mode
   alert pin becomes active low
*/
void Tmp112SetNormalPolarity(void)
{
  polarity = 0x00;
}


/*
   sets the polarity of the interrupt pin, in inverted mode
   alert become active hihg
*/
void Tmp112SetInvertedPolarity(void)
{
  polarity = 0x01;
}
/*
   Set the number of fault conditions required for generating
   an alert
*/
void Tmp112SetFaultQueue(byte queueSize)
{
  switch (queueSize)
  {
    case 1:
      faultQueue = 0x00;
      break;
    case 2:
      faultQueue = 0x01;
      break;
    case 4:
      faultQueue = 0x02;
      break;
    case 6:
      faultQueue = 0x03;
      break;
  }
}

/*
   Enabling one shot, TMP112 performs a single temperature conversion and next,
   disables one shot bit
*/
void Tmp112EnableOneShot(void)
{
  oneShot = 0x01;
}
/*
   enableing extended mode enables measuring temperatures above 128 C
   using 13-bit data format
*/
void Tmp112EnableExtendedMode(void)
{
  extendedMode = 0x01;
}
/*
   disbling extended mode sets the sensor use normal 12-but data format
*/
void Tmp112DisableExtendedMode(void)
{
  extendedMode = 0x00;
}

/*
   set the conversion rate from 0.25 Hz to 8 Hz
   0: 0.25 Hz
   1: 1 Hz
   2: 4 Hz (default)
   3: 8 Hz
*/
void Tmp112SetConversionRate(byte rate)
{
  switch (rate)
  {
    case 0:
      conversionRate = 0x00;
      break;
    case 1:
      conversionRate = 0x01;
      break;
    case 2:
      conversionRate = 0x02;
      break;
    case 3:
      conversionRate = 0x03;
      break;
  }
}


/*
   This function builds the unsigned short word for writing it
   to the configuration register
*/
unsigned short Tmp112CodeConfigurationRegister(void)
{
  unsigned short registerContent = 0x0000;
  registerContent |= (oneShot << 15);//((oneShot &= 0x01) << 15);
  registerContent |= (faultQueue << 11); //((faultQueue &= 0x03) << 11);
  registerContent |= (polarity << 10); //((polarity &= 0x01) << 10);
  registerContent |= (thermostatMode << 9); //((thermostatMode &= 0x01) << 9);
  registerContent |= (shutDownMode << 8); // ((shutDownMode &= 0x01) << 8);
  registerContent |= (conversionRate << 6); //((conversionRate &= 0x03) << 6);
  registerContent |= (extendedMode << 4); //((extendedMode &= 0x01) << 4);
  //  Serial.print("Building configuration register: ");
  //  Serial.println(registerContent, BIN);
  return registerContent;
}


/*
   This function reads and decodes the content of configuration register
   the return value should equal zero. A non-zero return value indicates
   a mismatch in the content of the register with the user defined
   configurations
*/
byte Tmp112DecodeConfigurationRegister(void)
{
  unsigned short registerContent = Tmp112ReadConfigurationRegister(); // read register values
  byte returnValue = 0x00;
  // validity check
  if (!(oneShot == (registerContent & (1 << 15)) >> 15))
  {
    returnValue++;
    oneShot = (registerContent & (1 << 15)) >> 15;
  }
  // update value
  converterResolution = (registerContent & (0x03 << 13)) >> 13;
  // validity check
  if (!(faultQueue == (registerContent & (0x03 << 11)) >> 11))
  {
    returnValue++;
    faultQueue = (registerContent & (0x03 << 11)) >> 11;
  }
  // validity check
  if (!(polarity == (registerContent & (1 << 10)) >> 10))
  {
    returnValue++;
    polarity = registerContent & (1 << 10) >> 10;
  }
  // validity check
  if (!(thermostatMode == (registerContent & (1 << 9)) >> 9))
  {
    returnValue++;
    thermostatMode = registerContent & (1 << 9) >> 9;
  }
  // validity check
  if (!(shutDownMode == (registerContent & (1 << 8)) >> 8))
  {
    returnValue++;
    shutDownMode = registerContent & (1 << 8) >> 8;
  }
  // validitu check
  if (!(conversionRate == (registerContent & (0x03 << 6)) >> 6))
  {
    returnValue++;
    conversionRate = registerContent & (0x03 << 6) >> 6;
  }
  // update
  alert = registerContent & (1 << 5) >> 5;
  // validity check
  if (!(extendedMode == (registerContent & (1 << 4)) >> 4))
  {
    returnValue++;
    extendedMode = registerContent & (1 << 4) >> 4;
  }
}

/*
   This function converts a floating point themperature into
   unsigned short word 16-bits
   the bit shifting is dependent on the sensor resolution
*/
unsigned short Tmp112TemperatureLimitHelper(float temperatureLimit)
{
  unsigned short temperature = 0x0000;
  if (temperatureLimit < 0)  {
    temperatureLimit *= -1; // absolute value
    temperature = (temperatureLimit) / (TMP112_LSB_VALUE);
    temperature = ~(temperature); // two's complement
    temperature += 0x0001;
  }
  else
  {
    temperature = (temperatureLimit) / (TMP112_LSB_VALUE);
  }
  if (extendedMode)
  {
    temperature = temperature << 3;
    return temperature;
  }
  else
  {
    temperature = temperature << 4;
    return temperature;
  }
}




/*
   Set the temperature ligh limit before writing it to the register
   The value is given as a floating point number in valid range from
   -55 to 128 C
*/
void Tmp112SetHighLimit(float temperatureLimit)
{
  tempHighLimit = Tmp112TemperatureLimitHelper(temperatureLimit);
}
/*
   Set the temperature low limit before writing it to the register
*/
void Tmp112SetLowLimit(float temperatureLimit)
{
  tempLowLimit = Tmp112TemperatureLimitHelper(temperatureLimit);
}


/*
   This function reads the configuration register and returns
   a 16-bit word containing the raw register content
*/
unsigned short Tmp112ReadConfigurationRegister(void)
{
  unsigned short registerContent = 0x0000;

  Wire.beginTransmission(TMP112_ADDRESS);
  Wire.write(TMP112_CONF_REGISTER); // pointer register
  Wire.endTransmission();

  Wire.requestFrom(TMP112_ADDRESS, 2);
  if (Wire.available() >= 2)
  {
    Serial.print("Reading configuration Register: ");
    registerContent |= Wire.read() << 8;
    registerContent |= Wire.read();
    Serial.println(registerContent, BIN);
  }
  return registerContent;
}


/*
   This function updates the raw  and float sensor data from the registers of the
   tmp112 sensor
*/
void Tmp112UpdateSensorData(void)
{
  unsigned short isNegative = 0x00;
  unsigned short registerContent = 0x0000;

  Wire.beginTransmission(TMP112_ADDRESS);
  Wire.write(TMP112_READ_TMP_REGISTER); // pointer register
  Wire.endTransmission();

  Wire.requestFrom(TMP112_ADDRESS, 2);
  if (Wire.available() >= 2)
  {
    registerContent |= (Wire.read() << 8);
    registerContent |= Wire.read();
  }
  if (extendedMode)
  {
    registerContent &= ~(0x0007);
    registerContent = registerContent >> 3;
    if ((registerContent & 0x2000) >> 12)
      isNegative = 1;
  }
  else
  {
    registerContent &= ~(0x000F);
    registerContent = registerContent >> 4;
    if ((registerContent & 0x1000) >> 11)
      isNegative = 1;
  }
  if (isNegative)
  {
    registerContent = registerContent - 1;
    registerContent = ~(registerContent);
    temperatureFloat = registerContent * TMP112_LSB_VALUE;
    temperatureFloat *= -1.0;
  }
  else
  {
    temperatureFloat = registerContent * TMP112_LSB_VALUE;
  }
}



/*
   This function writes the settings to the configuration register of
   the TMP112 sensor
*/
void  Tmp112WriteConfigurationRegister(void)
{
  unsigned short registerContent = Tmp112CodeConfigurationRegister();
  byte byte1 = 0x00;
  byte1 |= (byte)((registerContent & 0xFF00) >> 8);
  byte byte2 = 0x00;
  byte2 |= (byte)((registerContent & 0x00FF) >> 0);
  Wire.beginTransmission(TMP112_ADDRESS);
  Wire.write(TMP112_CONF_REGISTER); // pointer register
  Wire.write(byte1);
  Wire.write(byte2);
  Wire.endTransmission();
}

/*
   This function writes the temp high limit register
   the limit should be set first using the function Tmp112SetHighLimit()
*/
void Tmp112WriteTempHighLimitRegister(void)
{
  byte byte1 = 0x00;
  byte1 |= (byte)((tempHighLimit &= 0xFF00) >> 8);
  byte byte2 = 0x00;
  byte2 |= (byte)((tempHighLimit &= 0x00FF) >> 0);
  Wire.beginTransmission(TMP112_ADDRESS);
  Wire.write(TMP112_TEMP_HIGH_REGISTER);
  Wire.write(byte1);
  Wire.write(byte2);
  Wire.endTransmission();
}

/*
   This function writes the temp low limit register
   the limit should be set using the function Tmp112SetLowLimit()
*/
void Tmp112WriteTempLowLimitRegister(void)
{
  byte byte1 = 0x00;
  byte1 |= (byte)((tempLowLimit &= 0xFF00) >> 8);
  byte byte2 = 0x00;
  byte2 |= (byte)((tempLowLimit &= 0x00FF) >> 0);
  Wire.beginTransmission(TMP112_ADDRESS);
  Wire.write(TMP112_TEMP_LOW_REGISTER);
  Wire.write(byte1);
  Wire.write(byte2);
  Wire.endTransmission();
}

/*
   use these functions for getting the sensor values
*/
void Tmp112GetTemperatureFloatValue(float *temperature)
{
  Tmp112UpdateSensorData();
  *temperature = temperatureFloat;
}

void Tmp112GetTemperatureRawValue(unsigned short *temperature)
{
  Tmp112UpdateSensorData();
  *temperature = temperatureRaw;
}

void Tmp112GetTemperatureValues(unsigned short *rawValue, float *floatValue)
{
  Tmp112UpdateSensorData();
  *rawValue = temperatureRaw;
  *floatValue = temperatureFloat;
}

