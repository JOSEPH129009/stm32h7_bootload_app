

#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include "stdint.h"
#include "frame.h"
#include "main.h"
#include "constant.h"

extern OP_STATE bootloader_state;
extern OP_STATE old_bootloader_state;

extern volatile uint8_t recv_buf[sizeof(frame_format_t)];
extern volatile uint32_t recv_counter;

extern volatile uint32_t bootloader_flag;
extern uint32_t bootloader_error_flag;
extern volatile uint32_t serial_port_timer;
extern uint32_t address_counter;
#endif /*_VARIABLE_H_*/
