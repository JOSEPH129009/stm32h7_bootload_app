#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#define FLASH_APP_ADDR 0x08020000U   //bank 1 sector 1 total of 128k
#define FLASH_APP_END_ADDRESS (FLASH_BANK1_BASE+FLASH_BANK_SIZE-0x10u) /**< address = 0x80ffff0, Leave a little extra space at the end. */

typedef enum{
    FLASH_OK             = 0x00u, /**< The action was seccussful*/
    FLASH_ERROR_SIZE      = 0x01u, /**< The binary is too big. */
    FLASH_ERROR_WRITE     = 0x02u, /**< Writing failed. */
    FLASH_ERROR_READBACK  = 0x04u, /**< Writing was successful, but the content of the memory is wrong. */
    FLASH_ERROR           = 0xFFu  /**< Generic error. */
}Flash_Status;

typedef void (*frame_ID_function_t)(void);
void bootloader_main(void);
Flash_Status flash_erase_sector(uint8_t sector, uint8_t numberofsector);
Flash_Status flash_write (uint32_t address, uint32_t* data, uint32_t length);
#endif /*_BOOTLOADER_H_*/