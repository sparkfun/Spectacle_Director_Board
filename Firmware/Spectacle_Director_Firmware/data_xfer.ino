/****************************************************************************
 * data_xfer.ino
 * I2C transfer functions
 * Mike Hord @ SparkFun Electronics
 * 23 Jan 2017
 * https://github.com/sparkfun/Spectacle_Director_Board
 * 
 * These functions provide a means to transfer data across I2C to daughter
 * boards, by data width.
 * 
 * This code is beerware; if you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * ****************************************************************************/

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
