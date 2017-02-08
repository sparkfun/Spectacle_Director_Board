/****************************************************************************
 * programming.
 * Constants and defines for programming file.
 * Mike Hord @ SparkFun Electronics
 * 23 Jan 2017
 * https://github.com/sparkfun/Spectacle_Director_Board
 * 
 * Contains some of the constants and definitions needed by the programming file.
 * 
 * This code is beerware; if you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * ****************************************************************************/

#ifndef _programming_h_
#define _programming_h_

//State machine
#define STATE_START      0
#define STATE_DONE       1
#define STATE_CONTENT    2

//Special bytes in the communication protocol
#define BYTE_START      0x55
#define BYTE_ESCAPE      0x7d
#define BYTE_SEPARATOR    0x58

//Buffer sizes
#define FLASH_BUFFER_SIZE    4096

// Constants for FSK modulation. Hopefully their names are descriptive
//  enough. Basically, the FSK modulation scheme generates a pulse several
//  cycles long (based on the baud rate) at CARRIER_FREQ - MOD_FREQ to
//  represent a zero, or at CARRIER_FREQ + MOD_FREQ to represent a one.
#define CARRIER_FREQ    4000
#define MOD_FREQ        1000
#define BAUD            450
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

#endif

