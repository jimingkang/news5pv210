key.bin: start.o led.o key.o
	arm-linux-ld -Ttext 0x0 -o key.elf $^
	arm-linux-objcopy -O binary key.elf key.bin
	arm-linux-objdump -D key.elf > key.dis
	gcc mkv210_image.c -o mkx210
	./mkx210 key.bin 210.bin
	
%.o : %.S
	arm-linux-gcc -o $@ $< -c -nostdlib

%.o : %.c
	arm-linux-gcc -o $@ $< -c -nostdlib

clean:
	rm *.o *.elf *.bin *.dis mkx210 -f

	
	