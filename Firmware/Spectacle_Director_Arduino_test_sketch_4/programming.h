//State machine
#define STATE_START      0
#define STATE_DONE       1
#define STATE_CONTENT    2

//Special bytes in the communication protocol
#define BYTE_START      0x55
#define BYTE_ESCAPE      0x7d
#define BYTE_SEPARATOR    0x58

//Buffer sizes
#define USB_BUFFER_SIZE      128
#define FLASH_BUFFER_SIZE    4096

