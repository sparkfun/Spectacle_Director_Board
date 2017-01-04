#include <Wire.h>
#include "dbt.h"

#define AUD_ADDR      0x09
#define BTN_ADDR      0x0A
#define LED_ADDR      0x0B

void setup() 
{
  pinMode(7, OUTPUT);
  Wire.begin();
  Serial.begin(115200);

  // Configure the I2C addresses of the attached boards.
  configureBoard(AUD_ADDR);
  configureBoard(BTN_ADDR);
  configureBoard(LED_ADDR);
  if(isReady(AUD_ADDR)) Serial.println("Audio!");
  if(isReady(LED_ADDR)) Serial.println("LED!");
  if(isReady(BTN_ADDR)) Serial.println("Button!");
  
  // Teach the daughter board about the behaviors we want it to use.
  sendByte(AUD_ADDR, PROG_ENABLE_REG, 1);
  while (progReady(AUD_ADDR) == 0);
  audioInit(AUD_ADDR, 0, 500, 0, 0, 0, 0, 500, 0);
  audioInit(AUD_ADDR, 1, 500, 2, 0, 0, 0, 0, 0);
  audioInit(AUD_ADDR, 4, 500, 3, 0, 0, 0, 0, 0);
  sendByte(AUD_ADDR, PROG_ENABLE_REG, 0);
  
  sendByte(BTN_ADDR, PROG_ENABLE_REG, 1);
  while (progReady(BTN_ADDR) == 0);
  // INIT CODE GOES HERE
  buttonInit(BTN_ADDR, 0, CONTINUOUS, 0);
  buttonInit(BTN_ADDR, 1, PRESS, 1);
  buttonInit(BTN_ADDR, 2, PRESS, 2);
  buttonInit(BTN_ADDR, 3, PRESS, 3);
  buttonInit(BTN_ADDR, 4, RELEASE, 1);
  buttonInit(BTN_ADDR, 5, PRESS, 5);
  sendByte(BTN_ADDR, PROG_ENABLE_REG, 0);
  Serial.println("Button program succeeded!");

  sendByte(LED_ADDR, PROG_ENABLE_REG, 1);
  while (progReady(LED_ADDR) == 0);
  // INIT CODE GOES HERE
  // For twinkle - StripID, color, NA, likelihood of twinkle, # LEDs, min time between twinkles, max len of twinkle
  // For lightning - StripID, color, NA, likelihood of strobe, * LEDs, time between flashes
  ledInit(LED_ADDR, 0, 500, FLAME, 0, 0x0010ff3f, 0, 3, 5, 500, 0);
  ledInit(LED_ADDR, 1, 500, SET_PIXEL, 1, 0x0000ff00, 0, 1, 60, 0, 0);
  ledInit(LED_ADDR, 4, 500, SET_PIXEL, 1, 0x00000000, 0, 1, 60, 0, 0);
  sendByte(LED_ADDR, PROG_ENABLE_REG, 0);
  Serial.println("LED program succeeded!");
  digitalWrite(7, HIGH);
}

void loop() 
{
  for (int i = 0; i < 6; i++)
  {
    sendCmd(LED_ADDR, i, getMail(BTN_ADDR, i));
    sendCmd(AUD_ADDR, i, getMail(BTN_ADDR, i));
    delay(10);
    //sendCmd(LED_ADDR, i, 0);
    //getMail(BTN_ADDR, i);
  }
}

