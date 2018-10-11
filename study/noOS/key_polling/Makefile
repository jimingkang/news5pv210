key_pooling.bin: start.o led.o key.o
	arm-none-linux-gnueabi-ld -Ttext 0xd0020010 -o key_polling.elf $^
	arm-none-linux-gnueabi-objcopy  -O binary key_polling.elf key_polling.bin
	arm-none-linux-gnueabi-objdump -D key_polling.elf > key_polling_elf.dis
	gcc mkv210_image.c -o mkx210
	./mkx210 key_polling.bin 210.bin
	
%.o : %.S
	arm-none-linux-gnueabi-gcc -o $@ $< -c -nostdlib

%.o : %.c
	arm-none-linux-gnueabi-gcc -o $@ $< -c -nostdlib

clean:
	rm *.o *.elf *.bin *.dis mkx210 -f

	
