boolean configureBoard(byte address)
{
  while (isReady(0x08) == false)
  {
    Serial1.println("0x08 not found");
  }
  sendByte(0x08, I2C_ADDR_REG, address);
  sendByte(0x08, CONFIGURED_REG, 1);
  while (isReady(address) == false)
  {
    Serial1.println("new address not found");
  }
  Serial1.println("new address found!");
}

