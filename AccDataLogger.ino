#include <SPI.h>
#include <SD.h>
#include "Arduino.h"
#include <Wire.h>
#include <EnableInterrupt.h>
//#include <avr/wdt.h>

#include <RingBuf.h>

// PIN CONFIGURATION
#define ACC_DATAOUT 16//MOSI
#define ACC_DATAIN  14//MISO 
#define ACC_SPICLOCK 15//sck
#define ACC_SLAVESELECT A4//ss
#define ACC_INT1 9
#define ACC_INT2 8
#define TMP_INT 7
#define SD_SLAVESELECT 5



/*
   variables for normal functions
*/
unsigned long frameCounter = 0; // counting the number of interrupts
unsigned long maxFileSize = 10000000; // 10 Mt
unsigned short timeoutCounter = 0;
unsigned short fileCounter = 0;
bool accFlag = 0; // interrupt flag of bma2x2 sensor
float accTemp, tmp112Temp; // floating point variables for sensor data

byte folderCounter = 0;
byte sensorCounter = 0;
byte wmLevel = 24;
byte interrutpCounter = 0;
String filePath;


File logFile;

struct sensorData
{
  //unsigned long counter;
  byte xData;
  byte yData;
  byte zData;
};

RingBuf *accBuffer = RingBuf_new(sizeof(struct sensorData), 64);


void setup()
{
  Wire.begin();
  Serial.begin(9600);
  pinMode(ACC_DATAOUT, OUTPUT);
  pinMode(ACC_DATAIN, INPUT);
  pinMode(ACC_SPICLOCK, OUTPUT);
  pinMode(ACC_SLAVESELECT, OUTPUT);
  pinMode(SD_SLAVESELECT, OUTPUT);
  pinMode(ACC_INT1, INPUT);
  pinMode(ACC_INT2, INPUT);

  byte clr;
  // enable SPI in Master Mode with SCK = CK/4
  SPCR    = (1 << SPE) | (1 << MSTR);
  SPI.setClockDivider(SPI_CLOCK_DIV8); //HALOO vaihda kellotaajuuden kanssa.
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  //SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV8, MSBFIRST, SPI_MODE0));
  clr = SPSR;
  clr = SPDR;

  delay(5000);

  Serial.println(" -- SETUP INIT -- ");


  byte chipID = 0x00;
  Bma2x2ReadRegister(0x00, &chipID);
  Serial.print("Sensor ID: ");
  Serial.println(chipID, DEC);

  Bma2x2SetFifoWatermarkLevel(wmLevel);
  Bma2x2Init();
  delay(2000);
  Tmp112Init();
  delay(1000);
  InitializeSDCard();
  InitilaizeFolder();
  GetNewFilePath();
  delay(1000);
  if (!accBuffer)
  {
    Serial.println("Not enough memoy");
    while (1);
  }


  logFile = SD.open(filePath, FILE_WRITE);
  if (! logFile)
  {
    Serial.println("Not enough memoy");
    while (1);
  }
  enableInterrupt(TMP_INT, TmpInt, CHANGE);
  enableInterrupt(ACC_INT1, AccInt1, CHANGE);
  enableInterrupt(ACC_INT2, AccInt2, RISING);

  Serial.print("Watermark trigger level is  ");
  Serial.println(wmLevel);

  delay(100);

  //Serial.println("Clear FIFO buffer");
  //ClearBuffers();
  timeoutCounter = 0;
  accFlag = 1;
  Bma2x2GetTemperature(&accTemp);
  Tmp112GetTemperatureFloatValue(&tmp112Temp);;
  Serial.println(" -- INIT END -- ");
}


/*
   The main loop
*/
void loop()
{
  if (accFlag)
  {
    accFlag = 0;
    ReadSensors();
    interrutpCounter++;
    if (4 < interrutpCounter)
    {
      if (maxFileSize < logFile.size())
        GetNewFilePath();
      interrutpCounter = 0;
      logFile.close();
      logFile = SD.open(filePath, FILE_WRITE);
    }
    timeoutCounter = 0;
  }
  WriteDataToSD();
  //TimeoutCount();
}


void TimeoutCount(void)
{
  delay(1);
  timeoutCounter++;
  if (1024 < timeoutCounter)
  {
    accFlag = 1;
    timeoutCounter = 0;
    Serial.println("interrupts timeout");
  }
}


/*
   convert TMP112 raw temperature value to a floating point value
*/
float Tmp112TempToFloat(unsigned short tmp112Temp)
{
  byte isNegative = 0;
  float temperatureFloat = 0.0;
  tmp112Temp &= ~(0x000F);
  tmp112Temp = tmp112Temp >> 4;
  if ((tmp112Temp & 0x1000) >> 11)
    isNegative = 1;
  if (isNegative)
  {
    tmp112Temp = tmp112Temp - 1;
    tmp112Temp = ~(tmp112Temp);
    temperatureFloat = tmp112Temp * 0.0625;
    temperatureFloat *= -1.0;
  }
  else
  {
    temperatureFloat = tmp112Temp * 0.0625;
  }
  return temperatureFloat;
}

/*
   convert a BMA222E temperatrure raw value to a floating point value
*/
float AccTempToFloat( byte accTemp)
{
  signed char data = 0x00;
  data |= accTemp;
  float returnValue = (float)data;
  returnValue = 23.0 + 0.5 * returnValue;
}

/*
   Initialize SD card
   no exit if initialization failes
*/
void InitializeSDCard(void)
{
  Serial.print("Initializind SD card... ");
  if (!SD.begin(SD_SLAVESELECT))
  {
    Serial.println("Failed");
    while (1);
  }
  else
    Serial.println("OK");
}

/*
   function detects existing folder and creates always a new one when
   it is being called
   if creating fails, the function does not exit.
*/
void InitilaizeFolder(void)
{
  Serial.print("Initializing folder... ");
  String folderName = "log" + String(folderCounter);
  bool isFolder = SD.exists(folderName);
  while (isFolder)
  {
    folderCounter++;
    folderName = "log" + String(folderCounter);
    isFolder = SD.exists(folderName);
  }
  if (!SD.mkdir(folderName)) {
    Serial.println("Failed");
    while (1);
  }
  else
  {
    Serial.print("OK ");
    //Serial.println(folderName);
  }
}

/*
   This function creates a filename for logging
   the count starts from zero
   once maxmimun counter value (9999) is reached an new folder is created
   and the counter is being reseted
*/
void GetNewFilePath(void)
{
  filePath = "log" + String(folderCounter) + "/" + "file" + String(fileCounter) + ".txt";
  Serial.print("Writing to: ");
  Serial.println(filePath);
  fileCounter++;
  if (9999 < fileCounter)
  {
    InitilaizeFolder();
    fileCounter = 0;
    filePath = "log" + String(folderCounter) + "/" + "file" + String(fileCounter) + ".txt";
  }
}

/*
   Read the  raw acceleration data to data buffer.
   The function checks the FIFO fillup level and attempts to read it empty
   The temperature values are updated when 125 acc frames are being read (approx. 1 sec.)
*/
void ReadSensors(void)
{
  //  Serial.print("buffer level ");
  //  Serial.println(accBuffer->numElements(accBuffer));
  byte fifoOverrun, fifoLevel, lostFrames;
  lostFrames = 0;
  Bma2x2GetFifoStatus(&fifoOverrun, &fifoLevel); // check the fifo level and overrun status
  while (4 < fifoLevel)
  {
    lostFrames += Bma2x2ReadBurstToBuffer(4, accBuffer); // read the registers first
    sensorCounter += 4;
    Bma2x2GetFifoStatus(&fifoOverrun, &fifoLevel);
  }
  //Bma2x2ClearInterruptLatch();
  if ( 0 < lostFrames)
  {
    Serial.print("Lost ");
    Serial.print(lostFrames);
    Serial.print(" frames at ");
    Serial.println(frameCounter);
  }
  if (125 <= sensorCounter) {
    Bma2x2GetTemperature(&accTemp);
    Tmp112GetTemperatureFloatValue(&tmp112Temp);
    sensorCounter -= 125;
  }
  if (fifoOverrun)
  {
    Bma2x2WriteFifoConfig1Register(); // clear flag
    Serial.print("FIFO overrun at ");
    Serial.println(frameCounter);
  }
}


/*
   Function for writing buffered data to SD card
*/
void WriteDataToSD(void)
{
  if (!(accBuffer->isEmpty(accBuffer)))
  {
    struct sensorData data;
    if (logFile)
    {
      frameCounter++;
      accBuffer->pull(accBuffer, &data);
      logFile.print(frameCounter, DEC);
      logFile.print(';');
      logFile.print(tmp112Temp, 4);
      logFile.print(';');
      logFile.print(accTemp, 1);
      logFile.print(';');
      logFile.print(AccDataConversion(data.xData), 2);
      logFile.print(';');
      logFile.print(AccDataConversion(data.yData), 2);
      logFile.print(';');
      logFile.println(AccDataConversion(data.zData), 2);
    }
  }
  else
    TimeoutCount();
}

/*
   Function for conferting raw acceleration data into floating point value
*/
float AccDataConversion(byte rawData)
{
  signed char data = 0x00;
  data |= rawData;
  float tmpData = (float)data;
  tmpData = tmpData * 15.63;
  return tmpData;
}


/*
   Interrupt servicer routines
*/
void AccInt2()
{
  accFlag = 1;
}

void AccInt1()
{
  //Serial.println("int1 triggered");
}

void TmpInt()
{
  Serial.println("Temperature interrupt");
}



