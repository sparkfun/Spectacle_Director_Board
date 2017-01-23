/****************************************************************************
 * i2c_registers.h
 * I2C register definitions
 * Mike Hord @ SparkFun Electronics
 * 23 Jan 2017
 * https://github.com/sparkfun/Spectacle_Director_Board
 * 
 * Defines for the I2C address space on the daughter boards.
 * 
 * This code is beerware; if you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * ****************************************************************************/

#ifndef _i2c_registers_h_
#define _i2c_registers_h_

#define PROG_ENABLE_REG 200
#define PROG_READY_REG  201
#define DATA_READY_REG  202
#define INC_STRUCT_TOP  128
#define CONFIGURED_REG  0xfd
#define I2C_ADDR_REG    0xfe

#endif

