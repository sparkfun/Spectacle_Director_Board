#include <Wire.h>
#include "i2c_registers.h"
#include <SerialFlash.h>
#include <SPI.h>
#include "boards.h"

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
  // Initialize I/O pins
  pinMode(SIG_LED, OUTPUT);                       // Signal LED
  pinMode(VOUT_EN, OUTPUT);                       // Power on/off to daughter boards
  pinMode(LOAD_BTN, INPUT_PULLUP);                // Load new firmware trigger button
  pinMode(MEM_RST, OUTPUT);                       // Hold/reset pin for flash memory
  pinMode(MEM_CS, OUTPUT);                        // Chip select pin for flash memory
  
  // Initialize hardware
  Wire.begin();                                  // Start the I2C peripheral
  sercom3.disableWIRE();                         // Disable the I2C bus
  SERCOM3->I2CM.BAUD.bit.BAUD = 43;              // Set the I2C SCL frequency to 400kHz
  sercom3.enableWIRE();                          // Re-enable I2C bus

  Serial1.begin(115200);                         // Enable hardware serial port for debugging.
  Serial1.println("Online!");

  SerialFlash.begin(MEM_CS);                     // Start the Serialflash library
  
  digitalWrite(VOUT_EN, HIGH);                   // Turn on the power to the daughter boards.
  
  digitalWrite(MEM_RST, LOW);                    // Reset the flash memory controller
  delay(10);
  digitalWrite(MEM_RST, HIGH);
  delay(10);
  
  digitalWrite(SIG_LED, LOW);                    // Turn LED off. We'll turn it on later, or
                                                 //  flash it if we have problems to communicate

  uint8_t id[5];                                 // Check the ID bytes of the flash memory. This
  SerialFlash.readID(id);                        //  is a quick and easy way to make sure the flash
  Serial1.print(id[0], HEX);                     //  is online and working.
  Serial1.print(id[1], HEX);
  Serial1.print(id[2], HEX);
  Serial1.print(id[3], HEX);
  Serial1.println(id[4], HEX);

  Serial1.println("Setup complete!");

  // Our first branch: check to make sure flash file exists and has good data in it.
  //  The file will always be named "config.txt", and stores the configuration for this particular
  //  Spectacle implementation.
  if (!SerialFlash.exists(filename))           // If the file doesn't exist...
  {
    SerialFlash.eraseAll();                    // ...format the flash...
    
    while (!SerialFlash.ready()) 
    {
      Serial1.println("Formatting flash..."); // ...and print a serial message/
      blinkLED(250);                          // blink the LED twice per second
    }
    Serial1.println("Done formatting flash."); // issue a done message
    SerialFlash.createErasable(filename, 4096); // create the file
    receiveFile();                            // Go to the subroutine where we receive a new file.
  }
  else if (digitalRead(LOAD_BTN) == 0)       // If our user indicates that they wish to upload a
  {                                          //  new file by holding the load button on boot,
    Serial1.println("Load file mode");       //  print a message, erase the file, and go to the
    file = SerialFlash.open(filename);       //  load file subroutine.
    file.erase();
    file.close();
    receiveFile();
  }
  else                                      // Otherwise (most times), just boot up, load the
  {                                         //  config file, and call it good.
    Serial1.println("File exists!");
    loadFile();
  }

  Serial1.println("Setup finished!");
}

// loop() runs over and over and handles message distribution between daughter boards.
void loop() 
{
  Board *bdPtr;                 // Pointer to a board in the linked list of boards.
  bdPtr = firstBoard;           // This pointer starts at the top and is indexed as
  while (bdPtr != NULL)         // we traverse the list in this loop. After the last
  {                             // board, the pointer will be to NULL, and we start over.
    if (bdPtr->isInput() == 1)  // If the board is an input board (determined at object
    {                           // creation time during config file loading)...
      for (int i = 0; i < bdPtr->getNumChannels(); ++i) // ...iterate over the channels
      {                                               // in the board's list and copy the
        int tempChl = bdPtr->getChannel(i);           // info to our global channel.
        channels[tempChl] = getMail(bdPtr->getI2CAddr(), tempChl);
      }
    }
    bdPtr = bdPtr->getNextBoard(); // Then, go to the next board.
  }
  bdPtr = firstBoard;  // Start over at the beginning of the list.
  while (bdPtr != NULL)
  {
    if (bdPtr->isInput() == 0)
    {
      for (int i=0; i<bdPtr->getNumChannels(); ++i)
      {
        int tempChl = bdPtr->getChannel(i);  // This time, we're going to only send data
        sendCmd(bdPtr->getI2CAddr(), tempChl, channels[tempChl]);  // to channels that need it.
      }
    }
    bdPtr = bdPtr->getNextBoard();
  }
  delay(25);
}


