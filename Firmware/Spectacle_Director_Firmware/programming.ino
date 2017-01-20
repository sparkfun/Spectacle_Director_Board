#include "programming.h"

// Certain registers in the SAMD21 need synchronization before being
//  written. These two functions handle that.
// Sync AC
static __inline__ void ACsync() __attribute__((always_inline, unused));
static void   ACsync() {
  while (AC->STATUSB.bit.SYNCBUSY == 1);
}
// Sync GCLK
static __inline__ void syncGCLK() __attribute__((always_inline, unused));
static void syncGCLK() {
  while (GCLK->STATUS.bit.SYNCBUSY == 1);
}

// variable definitions
uint8_t AC0level  = 16;   // AC0 level
uint8_t AC1level  = 16;   // AC1 level
volatile uint8_t ACintflag = 0 ;  // Used to identify AC interupt source
long lastInt = 0;
long lastBit = 0;
long tsli = 0;
long tslb = 0;

#define CARRIER_FREQ    4000
#define MOD_FREQ        1000
#define BAUD            300
#define CARRIER_LEN     (1000000/CARRIER_FREQ) 
#define CARRIER_LEN_LO  (0.9*CARRIER_LEN)
#define CARRIER_LEN_HI  (1.1*CARRIER_LEN)
#define ZERO_FREQ       (CARRIER_FREQ - MOD_FREQ)
#define ONE_FREQ        (CARRIER_FREQ + MOD_FREQ)
#define ZERO_BIT_LEN    (1000000/ZERO_FREQ)
#define ZERO_BIT_LEN_LO (0.8*ZERO_BIT_LEN)
#define ZERO_BIT_LEN_HI (1.2*ZERO_BIT_LEN)
#define ONE_BIT_LEN     (1000000/ONE_FREQ)
#define ONE_BIT_LEN_LO  (0.8*ONE_BIT_LEN)
#define ONE_BIT_LEN_HI  (1.20*ONE_BIT_LEN)
#define BIT_LEN         (1000000/BAUD)

void receiveFile()
{
  setupAC(AC0level, AC1level);
  Serial1.println("Receive file");
  tsli = micros();
  
  uint8_t state = STATE_START;
  uint8_t fileSizeIndex = 0;
  uint32_t fileSize = 0;
  
//  char usbBuffer[USB_BUFFER_SIZE];
  uint8_t flashBuffer[FLASH_BUFFER_SIZE];
  
  uint16_t flashBufferIndex = 0;
  uint8_t filenameIndex = 0;
  
  uint32_t lastReceiveTime = millis();

  //We assume the serial receive part is finished when we have not received something for 30 seconds
  while((lastReceiveTime + 30000) > millis())
  {
    uint16_t available; // = Serial1.readBytes(usbBuffer, USB_BUFFER_SIZE);
    static bool notReceiving = true;
    bool newBit = false;
    static byte bitsReceived = 0;
    static unsigned int incByte = 0;
    static uint16_t lineStart = 0;
    static bool endBitSeen = false;
    char receivedByte = 0;
    //Serial1.println("WFD");
  
    if(ACintflag != 0) 
    {
      // Interrupt triggered - so do soemthng (maybe) 
      tsli = micros() - lastInt;
      lastInt = micros();
      ACintflag = 0;
      //Serial1.println("ACINT");

      if (notReceiving) // Watch for a start bit when not receiving.
      {
        if ((tsli > ZERO_BIT_LEN_LO) && (tsli < ZERO_BIT_LEN_HI))
        {
          notReceiving = false;
          lastBit=lastInt;
          incByte = 0;
          bitsReceived = 0;
        }
      }

      // Recognize a one or zero when received.
      else if (micros() - lastBit > (BIT_LEN))
      {
        if ((tsli > ZERO_BIT_LEN_LO) && (tsli < ZERO_BIT_LEN_HI))
        {
          //Serial1.println("0");
          bitsReceived++;
          incByte = incByte<<1;
          lastBit = lastInt;
        }
        else if ((tsli > ONE_BIT_LEN_LO) && (tsli < ONE_BIT_LEN_HI))
        {
          //Serial1.println("1");
          bitsReceived++;
          incByte = incByte<<1 | 1;
          lastBit = lastInt;
        }
      }

      // Recognize that a full byte has been received.
      if (bitsReceived == 10)
      {
        bitsReceived = 0;
        receivedByte = incByte>>2;
        //Serial1.print((char)receivedByte);
        available = 1;
        incByte = 0;
        notReceiving = true;
      }
    }    
    
    if (state == STATE_DONE) break;
    
    if (available)
    {
      //Serial1.println("DATA");
      available = 0;
      lastReceiveTime = millis();
      uint8_t b = receivedByte;
      
      if (state == STATE_START)
      {
        file = SerialFlash.open(filename);
        state = STATE_CONTENT;
      }
      if (state == STATE_CONTENT)
      {
        Serial1.print((char)b);
        if ((b == '\n') && (flashBuffer[flashBufferIndex - 1] == BYTE_SEPARATOR))
        {
          Serial1.println("End of file");
          file.write(flashBuffer, flashBufferIndex);
          file.close();
          flashBufferIndex = 0;
          state = STATE_DONE;
          break;
        }
        //Normal byte; add to buffer
        else 
        {
          // Checksum calculation. If the character before this one was a
          //  newline, this character is a checksum, and we need to check
          //  the last line received.
          if ((flashBuffer[flashBufferIndex-1] == '\n') &&
              (flashBufferIndex > lineStart))
          {
            uint8_t checksum = 0;
            for (int i = lineStart; i < flashBufferIndex; ++i)
            {
              checksum = checksum + flashBuffer[i];
            }
            Serial1.println(b);
            Serial1.println(checksum);
            if (checksum != b)
            {
              while(1)
              {
                delay(1000);
                blinkError(8);
              }
            }
            lineStart = flashBufferIndex;
          }
          else // NOT a checksum character, so stash it
          {
            flashBuffer[flashBufferIndex++] = b;
          }
        }
        if (flashBufferIndex >= FLASH_BUFFER_SIZE)
        {
          file.write(flashBuffer, FLASH_BUFFER_SIZE);
          flashBufferIndex = 0;
        }
      }
    }
  }
  Serial1.println("Out of while");
}

// Load file from flash into memory, then program the downstream boards with
//  data from the flash memory.
void loadFile()
{
  // open the file for reading
  file = SerialFlash.open(filename);
  // catch a file error, although it's very unlikely one could happen
  //  at this point in the code b/c of the way the earlier code works
  if (file) Serial1.println("File opened");
  else 
  {
    Serial1.println("File didn't open");
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
    Serial1.println("Good config data!");
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
    int temp = atol(buff);
    // Configures the current board to a new I2C address. 
    configureBoard(i2c_addr);
    if (temp != checkType(i2c_addr))
    {
      Serial1.println("Board type does not match!");
    }
    else
    {
      Serial1.println("Board type matches!");
    }

    // Add a new Board object to our linked list of extant boards.
    uint8_t isInput = 0;
    if (temp >= 128 )
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
          //Serial1.println(tempChl);
        }*/
        //Serial1.println(bdPtr->getNumChannels());
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
    // If 'Y' or 'V', we've reached the end of daughter board data and
    //  must either break from the function or move on to virtual board
    //  data parsing.
    else if (fileBuffer[i] == 'Y' ||
             fileBuffer[i] == 'V')
    {
      bdPtr->setChannels(channelList);
      bdPtr->setNumChannels(numChannels);
      /*for (int z = 0; z < bdPtr->getNumChannels(); ++z)
      {
        int tempChl = bdPtr->getChannel(z);
        //Serial1.println(tempChl);
      }*/
      //Serial1.println(bdPtr->getNumChannels());
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
        long temp = atol(buff);
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
            fileBuffer[i] == 'Y' ||
            fileBuffer[i] == 'V')
        {
          //Serial1.println(fileBuffer[i]);
          break;
        }
      }
      sendByte(i2c_addr, DATA_READY_REG, 1); // tell daughter board we've got
                                             //  a config set for it.
      //Serial1.println("waiting for daughter board");
      while (dataAccepted(i2c_addr) == 1); // Wait for data to be accepted by
                                            //  daughter board.
    }
    // ANY OTHER CHARACTER indicates a data integrity error.
    else
    {
      dataIntegrityError();
    }
  }

  // At this point, fileBuffer[i] should be either V or Y.
  //  If it's Y, we can quit because we're at the end of the board config
  //  data. If it's V, however, we have virtual board info to load.
  if (fileBuffer[i] == 'Y')
  {
    return;
  }
  else if (fileBuffer[i] == 'V')
  {
    // handle virtual board stuff here
    uint8_t *tempChlList;
    i+=2; // index past first 'V' character and \n to actual VBoard information.
    firstVBoard = new VBoard();
    VBoard *vBrdPtr = firstVBoard;
    while (1) // We'll break to get out of this loop when we hit 'Y'
    {
      tempChlList = new uint8_t [3];
      j = 0; // reset the short buffer's index
      while (fileBuffer[i] != '\n')
      {
        buff[j] = fileBuffer[i];
        j++;
        i++;
      }
      buff[j] = '\0'; // NULL terminate the string
      tempChlList[0] = atol(buff); // this is the primary source channel number.
      i++; // index past newline
      j = 0; // reset the short buffer's index
      while (fileBuffer[i] != '\n') // Now we're reading the second line of the
                                    // VBoard config, the mode.
      {
        buff[j] = fileBuffer[i];
        j++;
        i++;
      }
      buff[j] = '\0'; // NULL terminate the string
      vBrdPtr->setMode(atol(buff));
      Serial1.println(buff);
      i++; // index past newline
      // Here we have a decision to make. While all VBoards have a single channel
      //  and a mode, after that point they differ. 
      switch(vBrdPtr->getMode())
      {
        case INVERT: // for invert, we only get one more piece of data: the 
                     //  output channel.
          j = 0; // reset the short buffer's index
          while (fileBuffer[i] != '\n') // Now we're reading the second line of the
                                        // VBoard config, the mode.
          {
            buff[j] = fileBuffer[i];
            j++;
            i++;
          }
          buff[j] = '\0'; // NULL terminate the string
          tempChlList[1] = atol(buff);
          i++; // index past newline
          break;
        case AND:
        case OR:
        case XOR: // These three get TWO channels of info
          for (int x = 1; x <= 2; ++x)
          {
            j = 0; // reset the short buffer's index
            while (fileBuffer[i] != '\n') // Now we're reading the second line of the
                                          // VBoard config, the mode.
            {
              buff[j] = fileBuffer[i];
              j++;
              i++;
            }
            buff[j] = '\0'; // NULL terminate the string
            tempChlList[x] = atol(buff);
            i++; // index past newline
          }
          break;
        case RANDOM:
          j = 0;     // reset the short buffer's index
          while (fileBuffer[i] != '\n') // Now we're reading the second line of the
                                        // VBoard config, the mode.
          {
            buff[j] = fileBuffer[i];
            j++;
            i++;
          }
          buff[j] = '\0'; // NULL terminate the string
          vBrdPtr->setPeriod(atol(buff));
          i++; // index past newline
        break;
        case PERIODIC: // for periodic mode, we fetch two uint32_t values, period and persist
          j = 0;     // reset the short buffer's index
          while (fileBuffer[i] != '\n') // Now we're reading the second line of the
                                        // VBoard config, the mode.
          {
            buff[j] = fileBuffer[i];
            j++;
            i++;
          }
          buff[j] = '\0'; // NULL terminate the string
          vBrdPtr->setPeriod(atol(buff));
          i++; // index past newline
          j = 0;     // reset the short buffer's index
          while (fileBuffer[i] != '\n') // Now we're reading the second line of the
                                        // VBoard config, the mode.
          {
            buff[j] = fileBuffer[i];
            j++;
            i++;
          }
          buff[j] = '\0'; // NULL terminate the string
          vBrdPtr->setPersist(atol(buff));
          i++; // index past newline
          break;
        case CONSTANT:
          j = 0;     // reset the short buffer's index
          while (fileBuffer[i] != '\n') // Now we're reading the second line of the
                                        // VBoard config, the mode.
          {
            buff[j] = fileBuffer[i];
            j++;
            i++;
          }
          buff[j] = '\0'; // NULL terminate the string
          vBrdPtr->setValue(atoi(buff));
          i++; // index past newline
          break;
      }
      vBrdPtr->setChannels(tempChlList);
      // At this point, we've gone through the entire virtual board and should either see
      //  'Y' (for end of file) or 'V' (for next board)
      if (fileBuffer[i] == 'Y')
      {
        vBrdPtr->addVBoard(NULL);
        break;
      }
      else if (fileBuffer[i] == 'V')
      {
        //create a new VBoard here
        VBoard *tempVBoard;
        tempVBoard = new VBoard();
        vBrdPtr->addVBoard(tempVBoard);
        vBrdPtr = tempVBoard;
      }
      else
      {
        dataIntegrityError();
      }
    }
  }
  else // ANY OTHER CHARACTER is a data error.
  {
    dataIntegrityError();
  }
  
}

void dataIntegrityError()
{
  // ...erase the flash memory (takes about 5 seconds)
  SerialFlash.eraseAll();
  while (SerialFlash.ready() == false);

  // ...and then go into an infinite error code blink loop.
  while(1)
  {
  Serial1.println("Bad config data!");
  delay(1000);
  blinkError(4);
  }
}

//##############################################################
//  This is the interrupt handler. The name is fixed, but you can
//  do what you like in here
//##############################################################

void AC_Handler()
{
  ACintflag = 0;  // Clear any previous interupt
  ACintflag = REG_AC_INTFLAG; // Copy the interupt flag register
  REG_AC_INTFLAG = 0x03;  // Reset the interrupts by writing  1s
}

//  Setup the Analogue Comparator with threshold values
//  AC0level and AC1level in 64 steps to VDD = +3.3 V
void setupAC(uint8_t AC0level, uint8_t AC1level )
{
   // Set up the AC clocks
  syncGCLK();
  GCLK->CLKCTRL.reg = 0x4120; //enable GGCLK for AC_ana, CLKGEN1 = 32 kHz Xtal
  syncGCLK();
  REG_GCLK_CLKCTRL = 0x401F; //enable GGCLK for AC_dig, CLKGEN0 = 48 MHz PLL
  syncGCLK();
  // Set up the AC
  REG_PM_APBCMASK |= PM_APBCMASK_AC;        // Set the AC bit in the APBCMASK register
  REG_AC_CTRLA = 0x00;      // Disable comparator(s)
  ACsync();
  REG_AC_COMPCTRL0 = 0x00083524;  // MUXPOS = AIN3 pin int on rising
  ACsync();
  REG_AC_COMPCTRL1 = 0x00082524;  // MUXPOS = AIN2  pin, int on rising
  ACsync();
  REG_AC_INTENSET = 0x03;   // Both AC generate an interrupt
  REG_AC_WINCTRL = 0x00;    // Single comparator modes
  ACsync();
  REG_AC_SCALER0 = AC0level;    // Set threshold level
  REG_AC_SCALER1 = AC1level;    // Set threshold level
  // Enable InterruptVector
  NVIC_EnableIRQ(AC_IRQn);  // Enable intrupt vector
  ACsync();
  REG_AC_COMPCTRL0 |= 0x00001;
   ACsync();
  REG_AC_COMPCTRL1 |= 0x00001;
   ACsync();
  REG_AC_CTRLA = 0x02;      // Enable comparator(s)
  ACsync();
  REG_AC_INTFLAG = 0x03;  // Reset the interupts by writing  1s
}

// Read the Analogue Comparator values that exceed the threshold values
//0 = none, 1 = AC0, 2 = AC1, 3 = AC0 and AC1
uint8_t readAC()
{
  ACsync();
  uint8_t statusa = REG_AC_STATUSA & 0x03 ; 
  return statusa ;
}

