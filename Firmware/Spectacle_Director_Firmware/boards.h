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

