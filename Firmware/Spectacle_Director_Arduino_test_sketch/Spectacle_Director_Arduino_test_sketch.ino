#include <Wire.h>

#define PROG_ENABLE_REG 200
#define PROG_READY_REG  201
#define DATA_READY_REG  202
#define INC_STRUCT_TOP  128

enum {ACTIVE, INACTIVE, SIDE_A_UP, SIDE_B_UP, SIDE_C_UP, SIDE_D_UP,
      SIDE_TOP_UP, SIDE_BOTTOM_UP, WRITE_X, WRITE_Y, WRITE_Z};
      
enum {YES_INT, NO_INT};

void setup() 
{
  pinMode(7, OUTPUT);
  Wire.begin();
  Serial.begin(115200);
  // Teach the daughter board about the behaviors we want it to use.
  sendByte(0x09, PROG_ENABLE_REG, 1);
  while (progReady(0x09) == 0);
  xlInit(0x09, 0, ACTIVE, 1);
 /* xlInit(0x09, 1, ACTIVE, 0);
  xlInit(0x09, 2, WRITE_X, 0);
  xlInit(0x09, 3, WRITE_Y, 0);
  xlInit(0x09, 4, WRITE_Z, 0);*/
  sendByte(0x09, PROG_ENABLE_REG, 0);

  sendByte(0x08, PROG_ENABLE_REG, 1);
  while (progReady(0x08) == 0);
  audioInit(0x08, 0, 100, 0, NO_INT, 0, 10, 1000, 0);
  /*audioInit(0x08, 1, 100, 1, YES_INT, 0, 10, 1000, 0);
  audioInit(0x08, 2, 100, 2, YES_INT, 0, 10, 1000, 0);
  audioInit(0x08, 3, 100, 3, YES_INT, 0, 10, 1000, 0);
  audioInit(0x08, 4, 100, 4, YES_INT, 0, 10, 1000, 0);*/
  sendByte(0x08, PROG_ENABLE_REG, 0);
}

void loop() 
{
  for (int i = 0; i < 5; i++)
  {
    sendCmd(0x08, i, getMail(0x09, i));
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
  Serial.println(temp);
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


void xlInit(byte address, uint8_t channel, uint8_t mode, uint8_t momentary)
{
  sendByte(address, INC_STRUCT_TOP, channel);
  sendByte(address, INC_STRUCT_TOP + 1, mode);
  sendByte(address, INC_STRUCT_TOP + 2, momentary);
  sendByte(address, INC_STRUCT_TOP + 3, 0);
  sendByte(address, INC_STRUCT_TOP + 4, 0);
  sendByte(address, INC_STRUCT_TOP + 5, 0);
  sendWord(address, INC_STRUCT_TOP + 6, 0);
  sendWord(address, INC_STRUCT_TOP + 8, 0);
  sendWord(address, INC_STRUCT_TOP + 10, 0);
  sendLWord(address, INC_STRUCT_TOP + 12, 0);
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
