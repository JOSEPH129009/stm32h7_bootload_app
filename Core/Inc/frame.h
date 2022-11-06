#ifndef _FRAMES_H_
#define _FRAMES_H_

#include "stdint.h"

//delimeters
#define BL_START_OF_FRAME  0x55 
#define BL_END_OF_FRAME    0xFF  

//frame IDS
#define BL_HEADER       0x01 //sends info about firmware: size , version 
#define BL_STATUS_CHECK 0x02 //not used yet
#define BL_START_UPDATE 0x03 //not used yet
#define BL_PAYLOAD      0x04
#define BL_UPDATE_DONE  0x05
#define BL_ACK_FRAME    0x06
#define BL_NACK_FRAME   0x07
#define PAYLOAD_LEN 256

//frame formated struct 274byte
typedef struct __attribute__((packed))
{
    uint8_t start_of_frame;
    uint8_t frame_id;
    uint16_t payload_len;
    uint8_t payload[PAYLOAD_LEN];
    uint8_t checksum; // 2's complement
    uint8_t end_of_frame;
}frame_format_t;


#endif /*_FRAMES_H_*/