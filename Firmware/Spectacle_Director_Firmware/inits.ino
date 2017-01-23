/****************************************************************************
 * inits.ino
 * Functions for configuring boards
 * Mike Hord @ SparkFun Electronics
 * 23 Jan 2017
 * https://github.com/sparkfun/Spectacle_Director_Board
 * 
 * Configuration functions for daughter boards
 * 
 * This code is beerware; if you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * ****************************************************************************/

// configureBoard() assigns an unconfigured (newly powered) daughter board a new
//  I2C address, and sets its CONFIGURED_REG value to 1 so it knows that it has
//  been reconfigured and can activate the downstream daughter boards.
boolean configureBoard(byte address)
{
  // Check to see if any unconfigured boards are on the bus. Wait for them, if
  //  none are available.
  while (isReady(0x08) == false)
  {
    Serial1.println("0x08 not found");
  }
  // When an unconfigured board becomes available, set its I2C address to the
  //  new address passed to this function, then set its CONFIGURED_REG to 1.
  sendByte(0x08, I2C_ADDR_REG, address);
  sendByte(0x08, CONFIGURED_REG, 1);
  // Check to make sure the new board has acquired its new address.
  while (isReady(address) == false)
  {
    Serial1.println("new address not found");
  }
  Serial1.println("new address found!");
}

