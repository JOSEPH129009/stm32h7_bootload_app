#ifndef INC_BOOTLOADER_H_
#define INC_BOOTLOADER_H_

#define FLASH_APP_ADDR 0x08020000U   //bank 1 sector 1 total of 128k
#define FLASH_APP_END_ADDRESS (FLASH_BANK1_BASE+FLASH_BANK_SIZE-0x10u) /**< address = 0x80ffff0, Leave a little extra space at the end. */

typedef enum{
    FLASH_OK             = 0x00u, /**< The action was seccussful*/
    FLASH_ERROR_SIZE      = 0x01u, /**< The binary is too big. */
    FLASH_ERROR_WRITE     = 0x02u, /**< Writing failed. */
    FLASH_ERROR_READBACK  = 0x04u, /**< Writing was successful, but the content of the memory is wrong. */
    FLASH_ERROR           = 0xFFu  /**< Generic error. */
}Flash_Status;

static void print(char *msg, ...);
static void jump_to_user_app(void);
void bootloader_main(void);

#endif /*INC_BOOTLOADER_H_*/