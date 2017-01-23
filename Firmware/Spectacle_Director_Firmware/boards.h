/****************************************************************************
 * boards.h
 * Daughter and virtual board class header file
 * Mike Hord @ SparkFun Electronics
 * 23 Jan 2017
 * https://github.com/sparkfun/Spectacle_Director_Board
 * 
 * This file provides class declarations for the Board and VBoard classes,
 * which are the implementations of real and virtual boards in a spectacle
 * system. One object of class Board will be created at runtime for each
 * daughter board, and one object of class VBoard will be created for each
 * virtual board.
 * 
 * This code is beerware; if you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * ****************************************************************************/

#ifndef _boards_h_
#define _boards_h_

class Board
{
  public:
  Board(uint8_t isInput, uint8_t i2cAddr);
  uint8_t isInput();
  uint8_t getI2CAddr();
  void setNumChannels(uint8_t numChannels);
  uint8_t getNumChannels();
  void setChannels(uint8_t *channels);
  uint8_t getChannel(uint8_t channel);
  void addBoard(Board *nextBoard);
  Board* getNextBoard();
  private:
  Board *nextBoard;
  uint8_t input;
  uint8_t i2cAddr;
  uint8_t numChannels;
  uint8_t *channels;
};

class VBoard
{
  public:
  VBoard();
  void setMode(uint8_t mode);
  uint8_t getMode();
  void setChannels(uint8_t *channels);
  uint8_t getChannel(uint8_t channel);
  void setPeriod(uint32_t period);
  uint32_t getPeriod();
  void setPersist(uint32_t persist);
  uint32_t getPersist();
  void setLastPeriod(uint32_t period);
  uint32_t getLastPeriod();
  void setLastPersist(uint32_t persist);
  uint32_t getLastPersist();
  void setValue(uint16_t value);
  uint16_t getValue();
  void addVBoard(VBoard *nextVBoard);
  VBoard* getNextVBoard();
  
  private:
  uint8_t mode;
  uint32_t period;
  uint32_t lastPeriod;
  uint32_t persist;
  uint32_t lastPersist;
  uint32_t value;
  uint8_t *channels;
  VBoard *nextVBoard;
};
enum {INVERT, AND, OR, XOR, RANDOM, RANDOM_TRIGGER, PERIODIC, CONSTANT};
#endif
