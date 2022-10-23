

#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include "stdint.h"
#include "frame.h"
extern volatile uint8_t recv_buf[sizeof(frame_format_t)];
extern volatile uint32_t recv_counter;

extern uint32_t bootloader_flag;
#endif /*_VARIABLE_H_*/
