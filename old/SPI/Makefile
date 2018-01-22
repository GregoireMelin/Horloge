all :
	@avr-gcc -Os -mmcu=atmega328p -DF_CPU=16000000 main.c 
	@avr-objcopy -O binary a.out a.bin
install:
	avrdude -c avrisp2 -p m328p -P usb -U flash:w:a.bin

clean:
	@rm *.out *.bin
