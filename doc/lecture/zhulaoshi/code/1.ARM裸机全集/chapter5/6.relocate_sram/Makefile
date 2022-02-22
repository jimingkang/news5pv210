led.bin: start.o led.o
	 arm-none-linux-gnueabi-ld -Tlink.lds -o led.elf $^
	 arm-none-linux-gnueabi-objcopy -O binary led.elf led.bin
	 arm-none-linux-gnueabi-objdump -D led.elf > led_elf.dis
	gcc mkv210_image.c -o mkx210
	./mkx210 led.bin 210.bin
	
%.o : %.S
	 arm-none-linux-gnueabi-gcc -o $@ $< -c -nostdlib

%.o : %.c
	 arm-none-linux-gnueabi-gcc -o $@ $< -c -nostdlib

clean:
	rm *.o *.elf *.bin *.dis mkx210 -f

	
	