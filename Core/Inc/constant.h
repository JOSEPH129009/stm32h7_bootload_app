
#ifndef _CONSTANT_H_
#define _CONSTANT_H_

#include "bit_field.h"

/*bootloader_flag*/

#define FirstByteRecv      bit0
#define UpdateInfoGet      bit1
#define UpdateDone         bit2
#define UpdateDoneReply    bit3

/*bootloader_error_flag*/
#define WrongUpdateInfo    bit0
#define FrameError         bit1
#define NoHeaderInfo       bit2
#define TimeOut            bit3
#define UpdateError        bit4

#endif /* _CONSTANT_H_ */