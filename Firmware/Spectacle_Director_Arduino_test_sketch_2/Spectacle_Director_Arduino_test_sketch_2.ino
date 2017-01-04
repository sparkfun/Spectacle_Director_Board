#include <Wire.h>

#define PROG_ENABLE_REG 200
#define PROG_READY_REG  201
#define DATA_READY_REG  202
#define INC_STRUCT_TOP  128
#define CONFIGURED_REG  0xfd
#define I2C_ADDR_REG    0xfe
#define BUTTON_ADDR     0x09
#define AUDIO_ADDR      0x0A

enum {PRESS, RELEASE, PRESS_RELEASE, CONTINUOUS, LATCHING};
      
enum {YES_INT, NO_INT};

void setup() 
{
  pinMode(7, OUTPUT);
  Wire.begin();
  Serial.begin(115200);
  configureBoard(BUTTON_ADDR);
  configureBoard(AUDIO_ADDR);
  // Teach the daughter board about the behaviors we want it to use.
  sendByte(BUTTON_ADDR, PROG_ENABLE_REG, 1);
  while (progReady(BUTTON_ADDR) == 0);
  buttonInit(BUTTON_ADDR, 0, PRESS_RELEASE, 8);
 /* xlInit(BUTTON_ADDR, 1, ACTIVE, 0);
  xlInit(BUTTON_ADDR, 2, WRITE_X, 0);
  xlInit(BUTTON_ADDR, 3, WRITE_Y, 0);
  xlInit(BUTTON_ADDR, 4, WRITE_Z, 0);*/
  sendByte(BUTTON_ADDR, PROG_ENABLE_REG, 0);

  sendByte(AUDIO_ADDR, PROG_ENABLE_REG, 1);
  while (progReady(AUDIO_ADDR) == 0);
  audioInit(AUDIO_ADDR, 0, 100, 0, NO_INT, 0, 10, 1000, 0);
  /*audioInit(AUDIO_ADDR, 1, 100, 1, YES_INT, 0, 10, 1000, 0);
  audioInit(AUDIO_ADDR, 2, 100, 2, YES_INT, 0, 10, 1000, 0);
  audioInit(AUDIO_ADDR, 3, 100, 3, YES_INT, 0, 10, 1000, 0);
  audioInit(AUDIO_ADDR, 4, 100, 4, YES_INT, 0, 10, 1000, 0);*/
  sendByte(AUDIO_ADDR, PROG_ENABLE_REG, 0);
}

void loop() 
{
  for (int i = 0; i < 5; i++)
  {
    sendCmd(AUDIO_ADDR, i, getMail(BUTTON_ADDR, i));
  }
}
boolean configureBoard(byte address)
{
  while (isReady(0x08) == false)
  {
    Serial.println("0x08 not found");
  }
  sendByte(0x08, I2C_ADDR_REG, address);
  sendByte(0x08, CONFIGURED_REG, 1);
  while (isReady(address) == false)
  {
    Serial.println("new address not found");
  }
}
int getMail(byte address, byte mailbox)
{
  Wire.beginTransmission(address);
  Wire.write(mailbox * 2);
  Wire.endTransmission();
  Wire.requestFrom(address, 2);
  while (Wire.available() < 2);
  int temp = Wire.read();
  temp |= Wire.read()<<8;
  return temp;
}

void sendCmd(byte address,byte offset, uint16_t value)
{
  Wire.beginTransmission(address);
  Wire.write(2*offset);
  Wire.write(value);
  Wire.write(value>>8);
  Wire.endTransmission();
} 

void sendByte(byte address,byte offset, byte value)
{
  Wire.beginTransmission(address);
  Wire.write(offset);
  Wire.write(value);
  Wire.endTransmission();
} 

void sendWord(byte address,byte offset, uint16_t value)
{
  Wire.beginTransmission(address);
  Wire.write(offset);
  Wire.write(value);
  Wire.write(value>>8);
  Wire.endTransmission();
}

void sendLWord(byte address, byte offset, uint32_t value)
{
  Wire.beginTransmission(address);
  Wire.write(offset);
  Wire.write(value);
  Wire.write(value>>8);
  Wire.write(value>>16);
  Wire.write(value>>24);
  Wire.endTransmission();
}

byte progReady(byte address)
{
  byte resp = 0;
  Wire.beginTransmission(address);
  Wire.write(PROG_READY_REG);
  Wire.endTransmission();
  Wire.requestFrom(address, 1);
  while (Wire.available() == 0);
  return Wire.read();
}

byte dataAccepted(byte address)
{
  Wire.beginTransmission(address);
  Wire.write(DATA_READY_REG);
  Wire.endTransmission();
  Wire.requestFrom(address, 1);
  while (Wire.available() == 0);
  return Wire.read();
}

boolean isReady(byte address)
{
  Wire.requestFrom(address, 1);
  delay(5);
  if (Wire.available() > 0)
  {
    Wire.read();
    return true;
  }
  else
  {
    return false;
  }
}

void buttonInit(byte address, uint8_t channel, uint8_t mode, uint8_t button)
{
  sendByte(address, INC_STRUCT_TOP, channel);
  sendByte(address, INC_STRUCT_TOP + 1, mode);
  sendByte(address, INC_STRUCT_TOP + 2, button);
  sendByte(address, INC_STRUCT_TOP + 3, 0);
  sendByte(address, INC_STRUCT_TOP + 4, 0);
  sendByte(address, INC_STRUCT_TOP + 5, 0);
  sendLWord(address, INC_STRUCT_TOP + 6, 0);
  sendByte(address, DATA_READY_REG, 1);
  while (dataAccepted(address) == 1);
}

void audioInit(byte address, uint8_t channel, int16_t threshold, uint8_t soundIndex, 
               uint8_t interruptable, uint8_t pending, int32_t predelay, 
               int32_t postdelay, int32_t audioTimer)
{
  sendByte(address, INC_STRUCT_TOP, channel);
  sendWord(address, INC_STRUCT_TOP + 1, threshold);
  sendByte(address, INC_STRUCT_TOP + 3, soundIndex);
  sendByte(address, INC_STRUCT_TOP + 4, interruptable);
  sendWord(address, INC_STRUCT_TOP + 5, pending);
  sendWord(address, INC_STRUCT_TOP + 6, predelay);
  sendLWord(address, INC_STRUCT_TOP + 10, postdelay);
  sendLWord(address, INC_STRUCT_TOP + 14, audioTimer);
  sendLWord(address, INC_STRUCT_TOP + 18, 0);
  sendLWord(address, INC_STRUCT_TOP + 19, 0);
  
  sendByte(address, DATA_READY_REG, 1);
  while (dataAccepted(address) == 1);
}
