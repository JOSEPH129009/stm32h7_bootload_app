# stm32h7_bootload_app


- CH340 driver does not work so change USART1 pins and use FDTI chip to talk to /dev/ttyUSB0

- only takes program size smaller than 128k (a sector for STM32H743)
- if read 0xFFFFFFFF at the start of USER_APP_ADDRESS then it prints app not found

