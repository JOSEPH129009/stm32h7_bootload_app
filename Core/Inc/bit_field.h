
#ifndef _BIT_FIELD_H_
#define _BIT_FIELD_H_

#include "stdint.h"

#define bit0	0x1
#define bit1	0x2
#define bit2	0x4
#define bit3	0x8
#define bit4	0x10
#define bit5	0x20
#define bit6	0x40
#define bit7	0x80
#define bit8	0x100
#define bit9	0x200
#define bit10	0x400
#define bit11	0x800
#define bit12	0x1000
#define bit13	0x2000
#define bit14	0x4000
#define bit15	0x8000
#define bit16	0x10000
#define bit17	0x20000
#define bit18	0x40000
#define bit19	0x80000
#define bit20	0x100000
#define bit21	0x200000
#define bit22	0x400000
#define bit23	0x800000
#define bit24	0x1000000
#define bit25	0x2000000
#define bit26	0x4000000
#define bit27	0x8000000
#define bit28	0x10000000
#define bit29	0x20000000
#define bit30	0x40000000
#define bit31	0x80000000
uint32_t bit_on(uint32_t variable, uint32_t bit);
uint32_t bit_off(uint32_t variable, uint32_t bit);

typedef union Bit_operation_type
{
	uint32_t R;
	struct
	{
		uint32_t B00 : 1;
		uint32_t B01 : 1;
		uint32_t B02 : 1;
		uint32_t B03 : 1;
		uint32_t B04 : 1;
		uint32_t B05 : 1;
		uint32_t B06 : 1;
		uint32_t B07 : 1;
		uint32_t B08 : 1;
		uint32_t B09 : 1;
		uint32_t B10 : 1;
		uint32_t B11 : 1;
		uint32_t B12 : 1;
		uint32_t B13 : 1;
		uint32_t B14 : 1;
		uint32_t B15 : 1;
		uint32_t B16 : 1;
		uint32_t B17 : 1;
		uint32_t B18 : 1;
		uint32_t B19 : 1;
		uint32_t B20 : 1;
		uint32_t B21 : 1;
		uint32_t B22 : 1;
		uint32_t B23 : 1;
		uint32_t B24 : 1;
		uint32_t B25 : 1;
		uint32_t B26 : 1;
		uint32_t B27 : 1;
		uint32_t B28 : 1;
		uint32_t B29 : 1;
		uint32_t B30 : 1;
		uint32_t B31 : 1;
	}Bits;
}BIT_OP_TYPE;
#endif /* _BIT_FIELD_H_ */
