#include <SerialFlash.h>
#include <SPI.h>

const int FlashChipSelect = 10; // digital pin for flash chip CS pin
//const int FlashChipSelect = 21; // Arduino 101 built-in SPI Flash

void setup() {
  //uncomment these if using Teensy audio shield
  //SPI.setSCK(14);  // Audio shield has SCK on pin 14
  //SPI.setMOSI(7);  // Audio shield has MOSI on pin 7

  //uncomment these if you have other SPI chips connected
  //to keep them disabled while using only SerialFlash
  //pinMode(4, INPUT_PULLUP);
  //pinMode(10, INPUT_PULLUP);

  SerialUSB.begin(9600);

  // wait for Arduino Serial Monitor
  while (!SerialUSB) ;
  delay(100);
  SerialUSB.println("All Files on SPI Flash chip:");

  if (!SerialFlash.begin(FlashChipSelect)) {
    while (1) {
      SerialUSB.println("Unable to access SPI Flash chip");
      delay(2500);
    }
  }

  SerialFlash.opendir();
  int filecount = 0;
  while (1) {
    char filename[64];
    uint32_t filesize;

    if (SerialFlash.readdir(filename, sizeof(filename), filesize)) {
      SerialUSB.print("  ");
      SerialUSB.print(filename);
      SerialUSB.print(", ");
      SerialUSB.print(filesize);
      SerialUSB.print(" bytes");
      SerialFlashFile file = SerialFlash.open(filename);
      if (file) {
        unsigned long usbegin = micros();
        unsigned long n = filesize;
        char buffer[256];
        while (n > 0) {
          unsigned long rd = n;
          if (rd > sizeof(buffer)) rd = sizeof(buffer);
          file.read(buffer, rd);
          n = n - rd;
        }
        unsigned long usend = micros();
        SerialUSB.print(", read in ");
        SerialUSB.print(usend - usbegin);
        SerialUSB.print(" us, speed = ");
        SerialUSB.print((float)filesize * 1000.0 / (float)(usend - usbegin));
        SerialUSB.println(" kbytes/sec");
        file.close();
      } else {
        SerialUSB.println(" error reading this file!");
      }
      filecount = filecount + 1;
    } else {
      if (filecount == 0) {
        SerialUSB.println("No files found in SerialFlash memory.");
      }
      break; // no more files
    }
  }
}

void loop() 
{
  SerialUSB.println("herpyderpyderpderp");
  delay(10000);
}

