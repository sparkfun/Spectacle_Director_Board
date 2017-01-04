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
