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
  Serial.println("new address found!");
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
  while (dataAccepted(address) == 1)Serial.println("BUTTON");
}

void ledInit(byte address, uint8_t channel, int16_t threshold, uint8_t mode,
             uint8_t stringID, uint32_t color0, uint32_t color1, uint8_t pixel,
             uint8_t stringLen, uint32_t ledDelay, int8_t stepDelayMax)
{
  sendByte(address, INC_STRUCT_TOP, channel);
  sendWord(address, INC_STRUCT_TOP + 1, threshold);
  sendByte(address, INC_STRUCT_TOP + 3, mode);
  sendByte(address, INC_STRUCT_TOP + 4, stringID);
  sendLWord(address, INC_STRUCT_TOP + 5, color0);
  sendLWord(address, INC_STRUCT_TOP + 9, color1);
  sendByte(address, INC_STRUCT_TOP + 13, pixel);
  sendByte(address, INC_STRUCT_TOP + 14, stringLen);
  sendLWord(address, INC_STRUCT_TOP + 15, ledDelay);
  sendByte(address, INC_STRUCT_TOP + 19, stepDelayMax);
  for (int i = 20; i< sizeof(struct led); i++)
  {
    sendByte(address, INC_STRUCT_TOP + i, 0);
  }
  sendByte(address, DATA_READY_REG, 1);
  while (dataAccepted(address) == 1) Serial.println("LED");
}

