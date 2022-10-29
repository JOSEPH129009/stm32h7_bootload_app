### stm32h7_bootload_app Introdunction

- It waits 5s for update message. If no single byte is received by USART1, it looks to the memory location 0x0802_0000. If the data is 0xFFFFFFFF (erased value) at the location, the system halts in a while loop, else it performs a jump to the location. (have to enable a LED blinking to indicate where the syetem runs at cunrrently in the future)

- CH340 driver does not work so change USART1 pins and use FDTI chip to talk to /dev/ttyUSB0

- only takes program size smaller than 128k (a sector for STM32H743), update size does not have to aligned to a flash word (256bits), software will auto-fill up with 0xFF. max PAYLOAD of update byte is 256 bytes per frame

- implement program arguments with firmware version & update address in the future

- versioning are MAJOR.MINOR.PATCH. each is represented by 1 byte decimal value (0 ~ 255)
 e.g. v2.0.23 (update_info.version[0]=0x02, update_info.version[1]=0x00, update_info.version[2]=0x17)

- Now the program only erases and updates Bank 1 Sector 1 (0x802_0000), it needs further improvemnet for flexible sector update. 

### Toolkit

- this stm32H743zi bootloader program project was generated by STM32CubeMX makefile project. Using VScode IDE with Cortex-Debug, C/C++extension amd Makefile Tools.

- Jlink EDU Mini debugger/ FT232 USB UART Mini Board

- stm32h743 schematic:

- compiler: arm-none-eabi-gcc (15:10.3-2021.07-4) 10.3.1 20210621 (release)