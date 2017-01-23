/****************************************************************************
 * vBoardFuncs.ino 
 * Virtual board functions.
 * Mike Hord @ SparkFun Electronics
 * 23 Jan 2017
 * https://github.com/sparkfun/Spectacle_Director_Board
 * 
 * These functions represent the things that the virtual boards can do for
 * transforming channel inputs into different outputs.
 * 
 * This code is beerware; if you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * ****************************************************************************/
 
void vbAnd(uint8_t chlIn1, uint8_t chlIn2, uint8_t chlOut)
{
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
  if (channels[chlIn] <= 500)
  {
    channels[chlOut] = 1000 - channels[chlIn];
  }
  else
  {
    channels[chlOut] = channels[chlIn] - 1000;
  }
}

