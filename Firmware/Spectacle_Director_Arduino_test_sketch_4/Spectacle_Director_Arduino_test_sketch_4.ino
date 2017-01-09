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
  Serial1.begin(9600);

  SerialFlash.begin(10); // Start the Serialflash library, pin 10 as CS
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);      // Reset the flash memory controller
  delay(10);
  digitalWrite(A0, HIGH);
  delay(10);
  
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
}

void loop() 
{
  for (int i = 0; i < 5; i++)
  {
    sendCmd(AUD_ADDR, i, getMail(BTN_ADDR, i));
    delay(10);
  }
}


