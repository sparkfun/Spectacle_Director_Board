#include <Wire.h>
#include "dbt.h"
#include <SerialFlash.h>
#include <SPI.h>

#define AUD_ADDR      0x0A
#define MOT_ADDR      0x0A
#define BTN_ADDR      0x09
#define LED_ADDR      0x0C

#define MOSI 11
#define MISO 12
#define SCK  13

SerialFlashFile file;
char fileBuffer[4096];
char filename[11] = "config.txt";

void setup() 
{
  pinMode(7, OUTPUT);  // Signal LED
  pinMode(A1, INPUT_PULLUP);
  digitalWrite(7, LOW);
  pinMode(10, OUTPUT);
  Wire.begin();
  sercom3.disableWIRE();                         // Disable the I2C bus
  SERCOM3->I2CM.BAUD.bit.BAUD = 43;              // Set the I2C SCL frequency to 400kHz
  sercom3.enableWIRE();                          // Re-enable I2C bus
  Serial1.begin(115200);

  SerialFlash.begin(10); // Start the Serialflash library, pin 10 as CS
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);      // Reset the flash memory controller
  delay(10);
  digitalWrite(A0, HIGH);
  
  uint8_t id[5];
  SerialFlash.readID(id);
  Serial1.print(id[0], HEX);
  Serial1.print(id[1], HEX);
  Serial1.print(id[2], HEX);
  Serial1.print(id[3], HEX);
  Serial1.println(id[4], HEX);


  if (!SerialFlash.exists(filename))
  {
    while(!Serial1.available())
    {
      Serial1.println("File does not exist!");
      blinkError(5);
      delay(1000);
    }
    receiveFile();
  }
  else if (digitalRead(A1) == 0)
  {
    Serial1.println("Wait for formatting to push data file");
    receiveFile();
  }
  else
  {
    Serial1.println("File exists!");
    loadFile();
  }

  

  /*while(1);
  // Configure the I2C addresses of the attached boards.
  configureBoard(AUD_ADDR);
  configureBoard(MOT_ADDR);
  configureBoard(BTN_ADDR);
  configureBoard(LED_ADDR);
  if(isReady(AUD_ADDR)) Serial1.println("Audio!");
  if(isReady(MOT_ADDR)) Serial1.println("Motor!");
  if(isReady(BTN_ADDR)) Serial1.println("Button!");
  if(isReady(LED_ADDR)) Serial1.println("LED!");
  
  // Teach the daughter board about the behaviors we want it to use.
  sendByte(AUD_ADDR, PROG_ENABLE_REG, 1);
  while (progReady(AUD_ADDR) == 0);
  audioInit(AUD_ADDR, 0, 500, 0, 0, 0, 0, 500, 0);
  audioInit(AUD_ADDR, 1, 500, 2, 0, 0, 0, 0, 0);
  audioInit(AUD_ADDR, 4, 500, 3, 0, 0, 0, 0, 0);
  sendByte(AUD_ADDR, PROG_ENABLE_REG, 0);
  
  sendByte(MOT_ADDR, PROG_ENABLE_REG, 1);
  while (progReady(MOT_ADDR) == 0);
  servoInit(MOT_ADDR, 2, 500, 0, SWEEP_RETURN, 1000, 2000, 500, 1000);
  servoInit(MOT_ADDR, 3, 500, 0, WAG, 1000, 2000, 500, 1000);
  sendByte(MOT_ADDR, PROG_ENABLE_REG, 0);
  
  sendByte(BTN_ADDR, PROG_ENABLE_REG, 1);
  while (progReady(BTN_ADDR) == 0);
  // INIT CODE GOES HERE
  buttonInit(BTN_ADDR, 0, CONTINUOUS, 0);
  buttonInit(BTN_ADDR, 1, PRESS, 1);
  buttonInit(BTN_ADDR, 2, PRESS, 2);
  buttonInit(BTN_ADDR, 3, LATCHING, 3);
  buttonInit(BTN_ADDR, 4, RELEASE, 1);
  buttonInit(BTN_ADDR, 5, PRESS, 5);
  sendByte(BTN_ADDR, PROG_ENABLE_REG, 0);
  Serial1.println("Button program succeeded!");

  sendByte(LED_ADDR, PROG_ENABLE_REG, 1);
  while (progReady(LED_ADDR) == 0);
  // INIT CODE GOES HERE
  ledInit(LED_ADDR, 0, 500, FLAME, 0, 0x0010ff3f, 0, 3, 5, 500, 0);
  ledInit(LED_ADDR, 1, 500, SET_PIXEL, 1, 0x0000ff00, 0, 1, 60, 0, 0);
  ledInit(LED_ADDR, 4, 500, SET_PIXEL, 1, 0x00000000, 0, 1, 60, 0, 0);
  sendByte(LED_ADDR, PROG_ENABLE_REG, 0);
  Serial1.println("LED program succeeded!");
  digitalWrite(7, HIGH);
*/
}
void loop() 
{
  for (int i = 0; i < 1; i++)
  {
    sendCmd(AUD_ADDR, i, getMail(BTN_ADDR, i));
    delay(10);
    //sendCmd(LED_ADDR, i, 0);
    //getMail(BTN_ADDR, i);
  }
}


