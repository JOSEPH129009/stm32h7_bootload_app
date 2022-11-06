

#include "variable.h"

OP_STATE bootloader_state = {0};
OP_STATE old_bootloader_state = {0};

volatile uint8_t recv_buf[sizeof(frame_format_t)] = {0};
volatile uint32_t recv_uart_counter = 0;
/**
 * FirstByteRecv      bit0
 * UpdateInfoGet      bit1
 * UpdateDone         bit2
 * UpdateDoneReply    bit3
*/
volatile uint32_t bootloader_flag = 0;
/**
 * WrongUpdateInfo    bit0
 * FrameError         bit1
 * NoHeaderInfo       bit2
 * TimeOut            bit3
 * UpdateError        bit4
*/
uint32_t bootloader_error_flag = 0;
volatile uint32_t serial_port_timer = 0;

uint32_t address_counter = 0;

uint8_t transmit_buf[256] = {0};
uint16_t transmit_data_len = 0;