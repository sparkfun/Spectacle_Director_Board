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

