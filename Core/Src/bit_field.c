

#include "bit_field.h"

uint32_t bit_on(uint32_t variable, uint32_t bit) {
	BIT_OP_TYPE temp;
	temp.R = variable;
	switch (bit) {
	case bit0:
		temp.Bits.B00 = 1;
		break;
	case bit1:
		temp.Bits.B01 = 1;
		break;
	case bit2:
		temp.Bits.B02 = 1;
		break;
	case bit3:
		temp.Bits.B03 = 1;
		break;
	case bit4:
		temp.Bits.B04 = 1;
		break;
	case bit5:
		temp.Bits.B05 = 1;
		break;
	case bit6:
		temp.Bits.B06 = 1;
		break;
	case bit7:
		temp.Bits.B07 = 1;
		break;
	case bit8:
		temp.Bits.B08 = 1;
		break;
	case bit9:
		temp.Bits.B09 = 1;
		break;
	case bit10:
		temp.Bits.B10 = 1;
		break;
	case bit11:
		temp.Bits.B11 = 1;
		break;
	case bit12:
		temp.Bits.B12 = 1;
		break;
	case bit13:
		temp.Bits.B13 = 1;
		break;
	case bit14:
		temp.Bits.B14 = 1;
		break;
	case bit15:
		temp.Bits.B15 = 1;
		break;
	case bit16:
		temp.Bits.B16 = 1;
		break;
	case bit17:
		temp.Bits.B17 = 1;
		break;
	case bit18:
		temp.Bits.B18 = 1;
		break;
	case bit19:
		temp.Bits.B19 = 1;
		break;
	case bit20:
		temp.Bits.B20 = 1;
		break;
	case bit21:
		temp.Bits.B21 = 1;
		break;
	case bit22:
		temp.Bits.B22 = 1;
		break;
	case bit23:
		temp.Bits.B23 = 1;
		break;
	case bit24:
		temp.Bits.B24 = 1;
		break;
	case bit25:
		temp.Bits.B25 = 1;
		break;
	case bit26:
		temp.Bits.B26 = 1;
		break;
	case bit27:
		temp.Bits.B27 = 1;
		break;
	case bit28:
		temp.Bits.B28 = 1;
		break;
	case bit29:
		temp.Bits.B29 = 1;
		break;
	case bit30:
		temp.Bits.B30 = 1;
		break;
	case bit31:
		temp.Bits.B31 = 1;
		break;
	}
	return temp.R;
}
uint32_t bit_off(uint32_t variable, uint32_t bit) {
	BIT_OP_TYPE temp;
	temp.R = variable;
	switch (bit) {
	case bit0:
		temp.Bits.B00 = 0;
		break;
	case bit1:
		temp.Bits.B01 = 0;
		break;
	case bit2:
		temp.Bits.B02 = 0;
		break;
	case bit3:
		temp.Bits.B03 = 0;
		break;
	case bit4:
		temp.Bits.B04 = 0;
		break;
	case bit5:
		temp.Bits.B05 = 0;
		break;
	case bit6:
		temp.Bits.B06 = 0;
		break;
	case bit7:
		temp.Bits.B07 = 0;
		break;
	case bit8:
		temp.Bits.B08 = 0;
		break;
	case bit9:
		temp.Bits.B09 = 0;
		break;
	case bit10:
		temp.Bits.B10 = 0;
		break;
	case bit11:
		temp.Bits.B11 = 0;
		break;
	case bit12:
		temp.Bits.B12 = 0;
		break;
	case bit13:
		temp.Bits.B13 = 0;
		break;
	case bit14:
		temp.Bits.B14 = 0;
		break;
	case bit15:
		temp.Bits.B15 = 0;
		break;
	case bit16:
		temp.Bits.B16 = 0;
		break;
	case bit17:
		temp.Bits.B17 = 0;
		break;
	case bit18:
		temp.Bits.B18 = 0;
		break;
	case bit19:
		temp.Bits.B19 = 0;
		break;
	case bit20:
		temp.Bits.B20 = 0;
		break;
	case bit21:
		temp.Bits.B21 = 0;
		break;
	case bit22:
		temp.Bits.B22 = 0;
		break;
	case bit23:
		temp.Bits.B23 = 0;
		break;
	case bit24:
		temp.Bits.B24 = 0;
		break;
	case bit25:
		temp.Bits.B25 = 0;
		break;
	case bit26:
		temp.Bits.B26 = 0;
		break;
	case bit27:
		temp.Bits.B27 = 0;
		break;
	case bit28:
		temp.Bits.B28 = 0;
		break;
	case bit29:
		temp.Bits.B29 = 0;
		break;
	case bit30:
		temp.Bits.B30 = 0;
		break;
	case bit31:
		temp.Bits.B31 = 0;
		break;
	}
	return temp.R;
}
