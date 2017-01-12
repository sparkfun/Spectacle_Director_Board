Board::Board(uint8_t input, uint8_t i2cAddr)
{
  this->input = input;
  this->i2cAddr = i2cAddr;
}

uint8_t Board::isInput()
{
  return this->input;
}

uint8_t Board::getI2CAddr()
{
  return this->i2cAddr;
}

void Board::setNumChannels(uint8_t numChannels)
{
  this->numChannels = numChannels;
}

uint8_t Board::getNumChannels()
{
  return this->numChannels;
}

void Board::setChannels(uint8_t *channels)
{
  this->channels = channels;
}

uint8_t Board::getChannel(uint8_t channel)
{
  return this->channels[channel];
}

void Board::addBoard(Board *nextBoard)
{
  this->nextBoard = nextBoard;
}

Board* Board::getNextBoard()
{
  return this->nextBoard;
}

