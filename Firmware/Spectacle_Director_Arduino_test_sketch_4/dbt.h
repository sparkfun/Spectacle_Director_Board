#define PROG_ENABLE_REG 200
#define PROG_READY_REG  201
#define DATA_READY_REG  202
#define INC_STRUCT_TOP  128
#define CONFIGURED_REG  0xfd
#define I2C_ADDR_REG    0xfe

#define uint8 uint8_t
#define int8  int8_t
#define uint16 uint16_t
#define int16 int16_t
#define int32 int32_t
#define uint32 uint32_t

#pragma pack(1)
struct led
{
  uint8 channel;
  int16 threshold;
  uint8 mode;
  uint8 stringID;
  uint32 color0;
  uint32 color1;
  uint8 pixel;
  uint8 stringLen;
  int32 delay;
  int8 stepDelayMax;
  int8 stepDelay;
  int8 stepCntr;
  int32 ledTimer;
  int32 iteration;
  uint8 fading;
  uint8 inProcess;
  int8 direction;
  int8 redStep;
  int8 greenStep;
  int8 blueStep;
  int16 red;
  int16 green;
  int16 blue;
  int8 twinkleStepMax;
  int8 twinkleStep;

};
#pragma pack()

// Accelerometer constants
enum {ACTIVE, INACTIVE, SIDE_A_UP, SIDE_B_UP, SIDE_C_UP, SIDE_D_UP,
      SIDE_TOP_UP, SIDE_BOTTOM_UP, WRITE_X, WRITE_Y, WRITE_Z};
      
// Button constants
enum {PRESS, RELEASE, PRESS_RELEASE, CONTINUOUS, LATCHING};

// Audio constants
enum {YES_INT, NO_INT};

// LED constants
enum {SET_COLOR, SET_PIXEL, FADE_STRING, PARTIAL_FILL, RAINBOW,
      THEATER_CHASE, SCAN, TWINKLE, LIGHTNING, FLAME};

// Motor constants
enum {SWEEP_RETURN, SWEEP, WAG, GOTO};
