void vbAnd(uint8_t chlIn1, uint8_t chlIn2, uint8_t chlOut)
{
  Serial1.println("FRAUD");
  Serial1.println(chlIn1);
  Serial1.println(chlIn2);
  Serial1.println(chlOut);
  if (channels[chlIn1] == 1000 && channels[chlIn2] == 1000)
  {
    channels[chlOut] = 1000;
  }
  else
  {
    channels[chlOut] = 0;
  }
}

void vbOr(uint8_t chlIn1, uint8_t chlIn2, uint8_t chlOut)
{
  if (channels[chlIn1] == 1000 || channels[chlIn2] == 1000)
  {
    channels[chlOut] = 1000;
  }
  else
  {
    channels[chlOut] = 0;
  }
}

void vbXor(uint8_t chlIn1, uint8_t chlIn2, uint8_t chlOut)
{
  if (channels[chlIn1] != channels[chlIn2])
  {
    channels[chlOut] = 1000;
  }
  else
  {
    channels[chlOut] = 0;
  }
}

void vbInvert(uint8_t chlIn, uint8_t chlOut)
{
  if (channels[chlIn] == 1000)
  {
    channels[chlOut] = 0;
  }
  else
  {
    channels[chlOut] = 1000;
  }
}

