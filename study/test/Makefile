arm-none-linux-gnueabi-as led.S  -o led.o
arm-none-linux-gnueabi-ld -Ttext=0x30000000 led.o -o led
arm-none-linux-gnueabi-objcopy -I elf32-littlearm -O binary led /tftpboot/led.bin
