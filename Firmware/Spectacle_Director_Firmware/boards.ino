/****************************************************************************
 * boards.ino
 * Board and VBoard definition file
 * Mike Hord @ SparkFun Electronics
 * 23 Jan 2017
 * https://github.com/sparkfun/Spectacle_Director_Board
 * 
 * This file implements the class functions defined in boards.h. See that
 * file for more information about the classes and what they're for.
 * 
 * Resources:
 * <additional library requirements>
 * 
 * Development environment specifics:
 * <arduino/development environment version>
 * <hardware version>
 * <etc>
 * 
 * This code is beerware; if you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * ****************************************************************************/

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

VBoard::VBoard()
{
  this->channels = new uint8_t [3];
}

void VBoard::setChannels(uint8_t *channels)
{
  this->channels = channels;
}

uint8_t VBoard::getChannel(uint8_t channel)
{
  return this->channels[channel];
}

void VBoard::setMode(uint8_t mode)
{
  this->mode = mode;
}

uint8_t VBoard::getMode()
{
  return this->mode;
}

void VBoard::setPeriod(uint32_t period)
{
  this->period = period;
}

uint32_t VBoard::getPeriod()
{
  return this->period;
}

void VBoard::setPersist(uint32_t persist)
{
  this->persist = persist;
}

uint32_t VBoard::getPersist()
{
  return this->persist;
}

void VBoard::setLastPeriod(uint32_t period)
{
  this->lastPeriod = period;
}

uint32_t VBoard::getLastPeriod()
{
  return this->lastPeriod;
}

void VBoard::setLastPersist(uint32_t persist)
{
  this->lastPersist = persist;
}

uint32_t VBoard::getLastPersist()
{
  return this->lastPersist;
}

void VBoard::setValue(uint16_t value)
{
  this->value = value;
}

uint16_t VBoard::getValue()
{
  return this->value;
}

void VBoard::addVBoard(VBoard *nextVBoard)
{
  this->nextVBoard = nextVBoard;
}

VBoard* VBoard::getNextVBoard()
{
  return this->nextVBoard;
}
