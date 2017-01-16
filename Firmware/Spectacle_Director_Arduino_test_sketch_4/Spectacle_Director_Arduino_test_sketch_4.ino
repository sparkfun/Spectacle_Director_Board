#include <Wire.h>
#include "dbt.h"
#include <SerialFlash.h>
#include <SPI.h>
#include "boards.h"

#define AUD_ADDR      0x0A
#define MOT_ADDR      0x0A
#define BTN_ADDR      0x09
#define LED_ADDR      0x0C

#define MOSI 11
#define MISO 12
#define SCK  13

#define VOUT_EN A3
/*
// Defines for prototype boards
#define MEM_CS  9
#define MEM_RST 4
#define SIG_LED 7
#define LOAD_BTN 6
*/
// Defines for protoboard
#define MEM_CS  10
#define MEM_RST A0
#define SIG_LED 7
#define LOAD_BTN A1

Board *firstBoard;
Board *lastBoard;
int16_t channels[64];

SerialFlashFile file;
char fileBuffer[4096];
char filename[11] = "config.txt";

void setup() 
{
  pinMode(SIG_LED, OUTPUT);  // Signal LED
  pinMode(VOUT_EN, OUTPUT);
  digitalWrite(VOUT_EN, HIGH);
  delay(1000);
  pinMode(LOAD_BTN, INPUT_PULLUP);
  digitalWrite(SIG_LED, LOW);
  pinMode(MEM_CS, OUTPUT);
  Wire.begin();
  sercom3.disableWIRE();                         // Disable the I2C bus
  SERCOM3->I2CM.BAUD.bit.BAUD = 43;              // Set the I2C SCL frequency to 400kHz
  sercom3.enableWIRE();                          // Re-enable I2C bus
  Serial1.begin(115200);
  Serial1.println("Online!");

  SerialFlash.begin(MEM_CS); // Start the Serialflash library, pin 10 as CS
  pinMode(MEM_RST, OUTPUT);
  digitalWrite(MEM_RST, LOW);      // Reset the flash memory controller
  delay(10);
  digitalWrite(MEM_RST, HIGH);
  delay(10);
  
  uint8_t id[5];
  SerialFlash.readID(id);
  Serial1.print(id[0], HEX);
  Serial1.print(id[1], HEX);
  Serial1.print(id[2], HEX);
  Serial1.print(id[3], HEX);
  Serial1.println(id[4], HEX);


  Serial1.println("Setup complete!");
  
  if (!SerialFlash.exists(filename))
  {
    SerialFlash.eraseAll();
    
    while (!SerialFlash.ready()) 
    {
      Serial1.println("Formatting flash...");
      blinkLED(250);
    }
    Serial1.println("Done formatting flash.");
    SerialFlash.createErasable(filename, 4096);
    receiveFile();
  }
  else if (digitalRead(LOAD_BTN) == 0)
  {
    Serial1.println("Load file mode");
    file = SerialFlash.open(filename);
    file.erase();
    file.close();
    receiveFile();
  }
  else
  {
    Serial1.println("File exists!");
    loadFile();
  }

  Serial1.println("Setup finished!");
}

void loop() 
{
  Board *bdPtr;
  bdPtr = firstBoard;
  while (bdPtr != NULL)
  {
    if (bdPtr->isInput() == 1)
    {
      for (int i = 0; i < bdPtr->getNumChannels(); ++i)
      {
        int tempChl = bdPtr->getChannel(i);
        channels[tempChl] = getMail(bdPtr->getI2CAddr(), tempChl);
      }
    }
    bdPtr = bdPtr->getNextBoard();
  }
  bdPtr = firstBoard;
  while (bdPtr != NULL)
  {
    if (bdPtr->isInput() == 0)
    {
      for (int i=0; i<bdPtr->getNumChannels(); ++i)
      {
        int tempChl = bdPtr->getChannel(i);
        sendCmd(bdPtr->getI2CAddr(), tempChl, channels[tempChl]);
      }
    }
    bdPtr = bdPtr->getNextBoard();
  }
  delay(25);
}


