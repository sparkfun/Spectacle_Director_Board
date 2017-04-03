/****************************************************************************
 * spectacle_comms.ino
 * Spectacle-specific communications functions
 * Mike Hord @ SparkFun Electronics
 * 23 Jan 2017
 * https://github.com/sparkfun/Spectacle_Director_Board
 * 
 * These functions are built to provide a front end for commonly used
 * communications functions within the Spectacle framework
 * 
 * This code is beerware; if you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * ****************************************************************************/

 // Gets a value on a channel from an input board
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

// Sends a value to a channel used by an output board.
void sendCmd(byte address,byte offset, uint16_t value)
{
  Wire.beginTransmission(address);
  Wire.write(2*offset);
  Wire.write(value);
  Wire.write(value>>8);
  Wire.endTransmission();
} 

// Checks whether daughter board is ready to be programmed.
//  PROG_READY_REG will be set to 1 when the daughter board is
//  ready to receive configuration data.
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

// Checks whether daughter board has accepted the programmed
//  data or not. DATA_READY_REG will be cleared when the
//  daughter board has finished writing its config data to
//  memory.
byte dataAccepted(byte address)
{
  Wire.beginTransmission(address);
  Wire.write(DATA_READY_REG);
  Wire.endTransmission();
  Wire.requestFrom(address, 1);
  while (Wire.available() == 0);
  return Wire.read();
}

// Is the board at <address> responding?
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

// Check the board type located at <address>. Used to make sure that
//  the boards that are physically present in the system match those
//  in the configuration settings.
byte checkType(byte address)
{
  Wire.beginTransmission(address);
  Wire.write(0xff);
  Wire.endTransmission();
  Wire.requestFrom(address, 1);
  while (Wire.available() == 0);
  return Wire.read();
}

