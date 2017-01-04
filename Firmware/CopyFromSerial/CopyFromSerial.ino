/*
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For more information, please refer to <http://unlicense.org>
 * -------------------------------------------------------------------------
 * 
 * This is example code to 1) format an SPI Flash chip, and 2) copy raw 
 * audio files (mono channel, 16 bit signed, 44100Hz) to it using the 
 * SerialFlash library.  The audio can then be played back using the 
 * AudioPlaySerialflashRaw object in the Teensy Audio library.
 * 
 * To convert a .wav file to the proper .RAW format, use sox:
 * sox input.wav -r 44100 -b 16 --norm -e signed-integer -t raw OUTPUT.RAW remix 1,2
 * 
 * Note that the OUTPUT.RAW filename must be all caps and contain only the following
 * characters: A-Z, 0-9, comma, period, colon, dash, underscore.  (The SerialFlash
 * library converts filenames to caps, so to avoid confusion we just enforce it here).
 * 
 * It is a little difficult to see what is happening; aswe are using the Serial port
 * to upload files, we can't just throw out debug information.  Instead, we use the LED
 * (pin 13) to convey state.
 * 
 * While the chip is being formatted, the LED (pin 13) will toggle at 1Hz rate.  When 
 * the formatting is done, it flashes quickly (10Hz) for one second, then stays on 
 * solid.  When nothing has been received for 3 seconds, the upload is assumed to be 
 * completed, and the light goes off.
 * 
 * Use the 'rawfile-uploader.py' python script (included in the extras folder) to upload
 * the files.  You can start the script as soon as the Teensy is turned on, and the
 * USB serial upload will just buffer and wait until the flash is formatted.
 * 
 * This code was written by Wyatt Olson <wyatt@digitalcave.ca> (originally as part 
 * of Drum Master http://drummaster.digitalcave.ca and later modified into a 
 * standalone sample).
 * 
 * Enjoy!
 */

#include <SerialFlash.h>
#include <SPI.h>

//Buffer sizes
#define USB_BUFFER_SIZE      128
#define FLASH_BUFFER_SIZE    4096

//Max filename length (8.3 plus a null char terminator)
#define FILENAME_STRING_SIZE      13

//State machine
#define STATE_START      0
#define STATE_DONE       1
#define STATE_CONTENT    2

//Special bytes in the communication protocol
#define BYTE_START      0x55
#define BYTE_ESCAPE      0x7d
#define BYTE_SEPARATOR    0x58


//SPI Pins (these are the values on the Audio board; change them if you have different ones)
#define MOSI              11
#define MISO              12
#define SCK               13
#define CSPIN             10
//#define CSPIN           21  // Arduino 101 built-in SPI Flash

void setup()
{
  Serial1.begin(115200);  //Teensy serial is always at full USB speed and buffered... the baud rate here is required but ignored
  //while(!Serial1);
  Serial1.println("FARTS");
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);
  delay(10);
  digitalWrite(A0, HIGH);

  SerialFlash.begin(CSPIN);

  //We start by formatting the flash...
  uint8_t id[5];
  SerialFlash.readID(id);
  Serial1.print(id[0], HEX);
  Serial1.print(id[1], HEX);
  Serial1.print(id[2], HEX);
  Serial1.print(id[3], HEX);
  Serial1.println(id[4], HEX);

  
  //We are now going to wait for the upload program
  //while(!Serial1.available());
  
  SerialFlashFile flashFile;
  
  uint8_t state = STATE_START;
  uint8_t escape = 0;
  uint8_t fileSizeIndex = 0;
  uint32_t fileSize = 0;
  char filename[FILENAME_STRING_SIZE] = "config.txt";
  
  char usbBuffer[USB_BUFFER_SIZE];
  uint8_t flashBuffer[FLASH_BUFFER_SIZE];
  
  uint16_t flashBufferIndex = 0;
  uint8_t filenameIndex = 0;
  
  uint32_t lastReceiveTime = millis();

  //We assume the serial receive part is finished when we have not received something for 3 seconds
  while(Serial1.available() || lastReceiveTime + 30000 > millis())
  {
    uint16_t available = Serial1.readBytes(usbBuffer, USB_BUFFER_SIZE);
    if (available)
    {
      lastReceiveTime = millis();
    }
    if (state == STATE_DONE) break;
    for (uint16_t usbBufferIndex = 0; usbBufferIndex < available; usbBufferIndex++)
    {
      uint8_t b = usbBuffer[usbBufferIndex];
      
      if (state == STATE_START)
      {
        SerialFlash.eraseAll();
        
        while (!SerialFlash.ready()) 
        {
          Serial1.println("Formatting flash...");
          delay(500);
        }
        Serial1.println("Done formatting flash.");
  SerialFlash.create(filename, 5000);
  flashFile = SerialFlash.open(filename);
  if (!flashFile) Serial1.println("Couldn't open file!");
        state = STATE_CONTENT;
      }
      else if (state == STATE_CONTENT)
      {
        if (b == BYTE_SEPARATOR)
        {
          Serial1.println("End of file");
          flashFile.write(flashBuffer, flashBufferIndex);
          flashFile.close();
          flashBufferIndex = 0;
          state = STATE_DONE;
          break;
        }
        //Normal byte; add to buffer
        else 
        {
          flashBuffer[flashBufferIndex++] = b;
          Serial1.print((char)b);
        }
        if (flashBufferIndex >= FLASH_BUFFER_SIZE)
        {
          flashFile.write(flashBuffer, FLASH_BUFFER_SIZE);
          flashBufferIndex = 0;
        }
      }
    }
  }

  Serial1.println("Done uploading!");

  if (SerialFlash.exists(filename))
  {
    Serial1.println("File exists");
    flashFile = SerialFlash.open(filename);
    if (flashFile) Serial1.println("File opened");
    else Serial1.println("File didn't open");
  }
  else
  {
    Serial1.println("File doesn't exist");
  }
  char buffer[256];
  
  int i=0;
  do
  {
    Serial1.println("File contents: ");
    flashFile.read(buffer, 256);
    for (i = 0; i<256; i++)
    {
      Serial1.print(buffer[i]);
      if (buffer[i] == 'Y') break;
    }
  } while(i==255);
}

void loop()
{
  Serial1.println("herpy derperson");
  delay(10000);
  //Do nothing.
}

void flushError()
{
  uint32_t lastReceiveTime = millis();
  char usbBuffer[USB_BUFFER_SIZE];
  //We assume the serial receive part is finished when we have not received something for 3 seconds
  while(Serial1.available() || lastReceiveTime + 3000 > millis())
  {
    if (Serial1.readBytes(usbBuffer, USB_BUFFER_SIZE))
    {
      lastReceiveTime = millis();
    }
  }
}

