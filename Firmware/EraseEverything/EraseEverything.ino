#include <SerialFlash.h>
#include <SPI.h>

const int FlashChipSelect = 10; // digital pin for flash chip CS pin
//const int FlashChipSelect = 21; // Arduino 101 built-in SPI Flash

SerialFlashFile file;

const unsigned long testIncrement = 4096;

void setup() {
  //uncomment these if using Teensy audio shield
  //SPI.setSCK(14);  // Audio shield has SCK on pin 14
  //SPI.setMOSI(7);  // Audio shield has MOSI on pin 7

  //uncomment these if you have other SPI chips connected
  //to keep them disabled while using only SerialFlash
  //pinMode(4, INPUT_PULLUP);
  //pinMode(10, INPUT_PULLUP);

  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);
  digitalWrite(A0, HIGH);
  Serial1.begin(115200);

  // wait up to 10 seconds for Arduino Serial Monitor
  unsigned long startMillis = millis();
  while (!Serial1 && (millis() - startMillis < 10000)) ;
  delay(100);

  SerialFlash.begin(FlashChipSelect);
  unsigned char id[5];
  SerialFlash.readID(id);
  unsigned long size = SerialFlash.capacity(id);

  if (size > 0) {
    Serial1.print("Flash Memory has ");
    Serial1.print(size);
    Serial1.println(" bytes.");
    Serial1.println("Erasing ALL Flash Memory:");
    // Estimate the (lengthy) wait time.
    Serial1.print("  estimated wait: ");
    int seconds = (float)size / eraseBytesPerSecond(id) + 0.5;
    Serial1.print(seconds);
    Serial1.println(" seconds.");
    Serial1.println("  Yes, full chip erase is SLOW!");
    SerialFlash.eraseAll();
    unsigned long dotMillis = millis();
    unsigned char dotcount = 0;
    while (SerialFlash.ready() == false) {
      if (millis() - dotMillis > 1000) {
        dotMillis = dotMillis + 1000;
        Serial1.print(".");
        dotcount = dotcount + 1;
        if (dotcount >= 60) {
          Serial1.println();
          dotcount = 0;
        }
      }
    }
    if (dotcount > 0) Serial1.println();
    Serial1.println("Erase completed");
    unsigned long elapsed = millis() - startMillis;
    Serial1.print("  actual wait: ");
    Serial1.print(elapsed / 1000ul);
    Serial1.println(" seconds.");
  }
}

float eraseBytesPerSecond(const unsigned char *id) {
  if (id[0] == 0x20) return 152000.0; // Micron
  if (id[0] == 0x01) return 500000.0; // Spansion
  if (id[0] == 0xEF) return 419430.0; // Winbond
  if (id[0] == 0xC2) return 279620.0; // Macronix
  return 320000.0; // guess?
}


void loop() {

}

