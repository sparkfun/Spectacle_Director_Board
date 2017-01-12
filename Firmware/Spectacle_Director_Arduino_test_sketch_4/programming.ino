#include "programming.h"

void receiveFile()
{
    
  uint8_t state = STATE_START;
  uint8_t fileSizeIndex = 0;
  uint32_t fileSize = 0;
  
  char usbBuffer[USB_BUFFER_SIZE];
  uint8_t flashBuffer[FLASH_BUFFER_SIZE];
  
  uint16_t flashBufferIndex = 0;
  uint8_t filenameIndex = 0;
  
  uint32_t lastReceiveTime = millis();

  //We assume the serial receive part is finished when we have not received something for 30 seconds
  while(SerialUSB.available() || lastReceiveTime + 30000 > millis())
  {
    uint16_t available = SerialUSB.readBytes(usbBuffer, USB_BUFFER_SIZE);
    if (available)
    {
      lastReceiveTime = millis();
    }
    if (state == STATE_DONE) break;
    for (uint16_t usbBufferIndex = 0; usbBufferIndex < available; usbBufferIndex++)
    {
      uint8_t b = usbBuffer[usbBufferIndex];
      
      if (state == STATE_START)
      {
        file = SerialFlash.open(filename);
        if (!file) SerialUSB.println("Couldn't open file!");
        state = STATE_CONTENT;
      }
      else if (state == STATE_CONTENT)
      {
        if (b == BYTE_SEPARATOR)
        {
          SerialUSB.println("End of file");
          file.write(flashBuffer, flashBufferIndex);
          file.close();
          flashBufferIndex = 0;
          state = STATE_DONE;
          break;
        }
        //Normal byte; add to buffer
        else 
        {
          flashBuffer[flashBufferIndex++] = b;
          SerialUSB.print((char)b);
        }
        if (flashBufferIndex >= FLASH_BUFFER_SIZE)
        {
          file.write(flashBuffer, FLASH_BUFFER_SIZE);
          flashBufferIndex = 0;
        }
      }
    }
  }
}

// Load file from flash into memory, then program the downstream boards with
//  data from the flash memory.
void loadFile()
{
  // open the file for reading
  file = SerialFlash.open(filename);
  // catch a file error, although it's very unlikely one could happen
  //  at this point in the code b/c of the way the earlier code works
  if (file) SerialUSB.println("File opened");
  else 
  {
    SerialUSB.println("File didn't open");
    while(1)
    {
      blinkError(6);
    }
  }

  // Read the file into a largish buffer.
  file.read(fileBuffer, 4096);

  // i will be the index that we use throughout this function to track
  //  where we are in the file buffer.
  int i = 0;
  // buff is a short buffer that we'll operate on as we parse the file.
  char buff[12];

  // Read the first line of the file and check to see if it matches the
  //  expected value (should read "SPEC")
  while (fileBuffer[i] != '\n')
  {
    buff[i] = fileBuffer[i];
    i++;
  }
  
  buff[i] = '\0';    // NULL terminate buff 
  i++;               // Index past the first newline in the buffer

  // if we FAIL our file integrity check...
  if (strcmp(buff, "SPEC") != 0)
  {
    dataIntegrityError();
  }
  else
  {
    SerialUSB.println("Good config data!");
  }

  // j will be used for a local index in buff to copy data out of
  //  filebuffer.
  int j = 0;

  // i2c_addr will be incremented with each new board that comes
  //  online. We'll connect to the board, assign it a new I2C address,
  //  then check that it matches the expected board ID from the file.
  int i2c_addr = 0x09;
  while (1)
  {
    // Read one line into the short buffer.
    while (fileBuffer[i] != '\n')
    {
      buff[j] = fileBuffer[i];
      j++;
      i++;
    }
    buff[j] = '\0'; // NULL terminate the string
    // Convert the data from an ascii string to an integer value.
    int temp = atoi(buff);
    // Configures the current board to a new I2C address. 
    configureBoard(i2c_addr);
    if (temp != checkType(i2c_addr))
    {
      SerialUSB.println("Board type does not match!");
    }
    else
    {
      SerialUSB.println("Board type matches!");
    }

    // Add a new Board object to our linked list of extant boards.
    uint8_t isInput = 0;
    if (temp >= 128)
    {
      isInput = 1;
    }
    if (i2c_addr == 0x09) // first time through this loop
    {
      firstBoard = new Board(isInput, i2c_addr);
      lastBoard = firstBoard;
    }
    else // subsequent passes through this loop
    {
      Board *tempBoard;
      tempBoard = new Board(isInput, i2c_addr);
      lastBoard->addBoard(tempBoard);
      lastBoard = tempBoard;
      lastBoard->addBoard(NULL);
    }
    i++; // index past the newline
    i2c_addr++;
    j=0;
    if (fileBuffer[i] == 'B') break;
  }
  
  i += 2; // index past 'B' and '\n'. We should now be looking at the first
          //  'N' char of the behavior list.
  i2c_addr = 0x08; // Start one address below the lowest i2c address so we
                   //  can add one at the beginning of the loop.

  Board* bdPtr = firstBoard;
  uint8_t *channelList=NULL;
  uint8_t numChannels = 0;
  // Now, we want to teach the boards about the behaviors we want them
  //  to implement. These are stored in the buffer with break characters
  //  'N' for new board, 'n' for new behavior. Each '\n' separated line
  //  after those two has a leading '1', '2', or '4', depending on the
  //  data type of the parameter it represents, followed by a period, then
  //  the actual data.
  while (1)
  {
    // There are three possibilities here: 'N', 'n', and 'Y'. 
    // If 'N', we're onto a new board and must correspondingly increase the
    //  i2c address by one.
    if (fileBuffer[i] == 'N')
    {
      if (i2c_addr > 0x08) // Anytime other than the first time, we need to
                           // clear the existing PROG_ENABLE bit before we
                           // change the I2C.
      {
        sendByte(i2c_addr, PROG_ENABLE_REG, 0);
        // We also need to advance the bdPtr to point to the next Board in
        //  our linked list and add the channelList to the board.
        bdPtr->setChannels(channelList);
        bdPtr->setNumChannels(numChannels);      
       /* for (int z = 0; z < bdPtr->getNumChannels(); ++z)
        {
          int tempChl = bdPtr->getChannel(z);
          //SerialUSB.println(tempChl);
        }*/
        //SerialUSB.println(bdPtr->getNumChannels());
        numChannels = 0;
        bdPtr = bdPtr->getNextBoard();
      }
      i += 2; // index past 'N' and '\n'.
      i2c_addr++;
      // Tell the daughter board that programming data is incoming.
      sendByte(i2c_addr, PROG_ENABLE_REG, 1);
      while(progReady(i2c_addr) == 0); // wait for the daughter board to
                                       // signal that it's ready for data
    }
    // If 'Y', we've reached the end of the configuration data and must break
    //  from this loop (and, ultimately, from the function).
    else if (fileBuffer[i] == 'Y')
    {
      bdPtr->setChannels(channelList);
      bdPtr->setNumChannels(numChannels);
      /*for (int z = 0; z < bdPtr->getNumChannels(); ++z)
      {
        int tempChl = bdPtr->getChannel(z);
        //SerialUSB.println(tempChl);
      }*/
      //SerialUSB.println(bdPtr->getNumChannels());
      sendByte(i2c_addr, PROG_ENABLE_REG, 0);
      break;
    }
    // If 'n', we must write the configuration data out to the board.
    else if (fileBuffer[i] == 'n')
    {
      i+=2; // Index past 'n' and '\n'.
      byte remoteAddr = 128; // starting location of the I2C remote memory area
                               //  that we'll be writing to.
      while (1) // do this until we break, upon discovery of an 'N', 'n', or 'Y'.
      {
        // First, we must figure out how many bytes we need to transmit.
        char bytes = fileBuffer[i];
        i+=2; // Index past the # of bytes and the period that follows it.
        // Read one line into the short buffer.
        j = 0; // reset the short buffer's index
        while (fileBuffer[i] != '\n')
        {
          buff[j] = fileBuffer[i];
          j++;
          i++;
        }
        buff[j] = '\0'; // NULL terminate the string
        // Convert the data from an ascii string to an integer value.
        long temp = atoi(buff);
        if (remoteAddr == 128) // For the first datapoint in the set, save
                               //  this as a channel number.
        {
          uint8_t *channelListTemp;
          channelListTemp = new uint8_t [++numChannels];
          for (int k = 0; k < numChannels-1; ++k)
          {
            channelListTemp[k] = channelList[k];
          }
          channelListTemp[numChannels-1] = temp;
          channelList = channelListTemp;
        }
        switch (bytes)
        {
          case '1':
            sendByte(i2c_addr, remoteAddr, temp);
            remoteAddr += 1;
          break;
          case '2':
            sendWord(i2c_addr, remoteAddr, temp);
            remoteAddr += 2;
          break;
          case '4':
            sendLWord(i2c_addr, remoteAddr, temp);
            remoteAddr += 4;
          break;
          default:
          dataIntegrityError();
        }
        i++; // index past the '\n' character
        if (fileBuffer[i] == 'N' ||
            fileBuffer[i] == 'n' ||
            fileBuffer[i] == 'Y')
        {
          //SerialUSB.println(fileBuffer[i]);
          break;
        }
      }
      sendByte(i2c_addr, DATA_READY_REG, 1); // tell daughter board we've got
                                             //  a config set for it.
      //SerialUSB.println("waiting for daughter board");
      while (dataAccepted(i2c_addr) == 1); // Wait for data to be accepted by
                                            //  daughter board.
    }
    // ANY OTHER CHARACTER indicates a data integrity error.
    else
    {
      dataIntegrityError();
    }
  }
 /* bdPtr = firstBoard;
  while (bdPtr != NULL)
  {
    for (int x = 0; x < bdPtr->getNumChannels(); ++x)
    {
      SerialUSB.println(bdPtr->getChannel(x));
    }
    bdPtr = bdPtr->getNextBoard();
  }*/
}

void dataIntegrityError()
{
  // ...erase the flash memory (takes about 5 seconds)
  SerialFlash.eraseAll();
  while (SerialFlash.ready() == false);

  // ...and then go into an infinite error code blink loop.
  while(1)
  {
  SerialUSB.println("Bad config data!");
  delay(1000);
  blinkError(4);
  }
}

