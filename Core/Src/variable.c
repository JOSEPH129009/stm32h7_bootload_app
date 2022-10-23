
#include "variable.h"

volatile uint8_t recv_buf[sizeof(frame_format_t)] = {0};
volatile uint32_t recv_counter = 0;

uint32_t bootloader_flag = 0;